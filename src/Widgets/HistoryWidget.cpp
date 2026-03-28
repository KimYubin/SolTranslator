// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include <QClipboard>
#include <QGridLayout>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStringListModel>
#include <QTimer>

#include <expected>

#include "HistoryListDelegate.h"
#include "HistoryModel.h"
#include "SolHashQueue.h"
#include "SolMainWidget.h"
#include "SolTranslatorCore.h"

#include "Managers/ConfigManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/LayoutTextEdit.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolWidgetFactory.h"

#include "Utils/SolLog.h"
#include "Utils/Tr.h"

using Sol::i18n;


HistoryListView::HistoryListView(QWidget* parent) : QListView(parent)
{}

HistoryListView::~HistoryListView()
{}

QColor HistoryListView::getItemColor(const Sol::ItemColorRole inColorRole) const
{
    switch (inColorRole)
    {
    case Sol::itemTextColorRole:          return _itemTextColor;
    case Sol::itemSelectionTextColorRole: return _itemSelectionTextColor;
    case Sol::itemHoverTextColorRole:     return _itemHoverTextColor;
    case Sol::itemDisableColorRole:       return _itemDisableColor;
    default: ;
    }
    return QColor();
}

HistoryWidget::HistoryWidget(QWidget* parent) : ISolWidget(parent)
{
    setupUI();

    _currentTextType = TextType::TranslateText;
}

HistoryWidget::~HistoryWidget()
{}

void HistoryWidget::setupUI()
{
    setObjectName("HistoryWidget");

    _mainLayout = new QGridLayout(this);
    _mainLayout->setSpacing(0);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _splitter = new QSplitter(Qt::Horizontal);
    _splitter->setChildrenCollapsible(false);
    // splitter->setOpaqueResize(false);

    _mainLayout->addWidget(_splitter);

    _historyListView = new HistoryListView(_splitter);
    _historyListView->setMinimumWidth(150);
    _historyListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _historyListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _historyListView->setUniformItemSizes(true);
    _historyListView->setLayoutMode(QListView::Batched);
    _historyListView->setBatchSize(10);

    _splitter->addWidget(_historyListView);

    _selectedTextEdit = new LayoutTextEdit(_splitter);
    _selectedTextEdit->setObjectName("historySelectedDetail");
    _selectedTextEdit->setMinimumWidth(150);

    // 복사 버튼
    SolButton* trCopy = SolWidgetFactory::createCopyButton(_selectedTextEdit, [this]()
    {
        const QModelIndex curIdx = _historyListView->currentIndex();
        const Sol::HistoryItemRole curRole = (_currentTextType == TextType::OriginText) ? Sol::SourceFullTextRole : Sol::TargetFullTextRole;

        return _historyListModel->data(curIdx, curRole).toString();
    });
    _selectedTextEdit->addBottomWidget(trCopy, 0, Qt::AlignLeft);

    // 원문/번역 토글
    {
        const SolButton* toggleButton = _selectedTextEdit->addBottomButton(QIcon(":/img/swap_text_img")
                                                                         , Qt::TabFocus
                                                                         , i18n(Tr::Source_Target_Toggle)
                                                                         , solConfig.shortcut(Action::HistoryToggle)
                                                                         , 0
                                                                         , Qt::AlignLeft);

        connect(toggleButton, &QPushButton::clicked, this, [this]() { toggleTranslationText(); });
    }

    // 기록 삭제
    {
        const SolButton* deleteButton = _selectedTextEdit->addBottomButton(QIcon(":/img/delete_img")
                                                                         , Qt::TabFocus
                                                                         , i18n(Tr::Delete_Translation)
                                                                         , QKeySequence()
                                                                         , 1
                                                                         , Qt::AlignRight);

        connect(deleteButton, &QPushButton::clicked, this, [this]()
        {
            const QModelIndex curIdx = _historyListView->currentIndex();
            const qlonglong dbId     = _historyListModel->data(curIdx, Sol::DbIdRole).toLongLong();
            solCore->historyManager()->asyncDeleteHistory(dbId);
            _selectedTextEdit->setText("");
        });
    }

    _splitter->addWidget(_selectedTextEdit);
    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 2);

    // _splitter->setSizes({250, 500});
    _historyListModel = new HistoryModel(this);

    auto proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_historyListModel);
    proxyModel->setFilterKeyColumn(0);

    _historyListView->setModel(proxyModel);
    _historyListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _historyListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _historyListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // select item
    QItemSelectionModel* selectionModel = _historyListView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& previous)
    {
        if (current.isValid() == false)
        {
            return;
        }

        const int lastestRowIndex  = current.row();
        const std::expected<const HistoryCacheData*, QString> selectedTr = _historyListModel->getTranslateCache(lastestRowIndex);

        if (selectedTr.has_value() == false)
        {
            solDebug << selectedTr.error();
            return;
        }

        // only db update
        const qint64 newCurrentTimelineId = selectedTr.value()->getTimelineId();
        if (newCurrentTimelineId == _currentTimelineId)
        {
            return;
        }
        _currentTimelineId = newCurrentTimelineId;
        _currentTimeStamp  = selectedTr.value()->getTimeStamp();
        _currentTextType   = TextType::TranslateText;

        _selectedTextEdit->setFormattingText(selectedTr.value()->getTargetText(), selectedTr.value()->getTextStyle());

        QTextCursor textCursor = _selectedTextEdit->textCursor();
        textCursor.setPosition(0);
        _selectedTextEdit->setTextCursor(textCursor);
    });


    connect(_historyListModel, &QAbstractItemModel::modelReset, this, [this]()
    {
        // Fix scrollbar
        const QScrollBar* scrollBar = _historyListView->verticalScrollBar();
        const qreal currentScroll   = scrollBar->value();
        const qreal maxScroll       = scrollBar->maximum();
        _listScrollBarRatio         = (maxScroll > 0) ? (currentScroll / maxScroll) : 0.0;

        // Restore previous selection
        if (_currentTimelineId < 0)
        {
            return;
        }

        const std::expected<int, QString> findIdx = solCore->historyManager()->findModelIdxFromTimelineId(_currentTimelineId, _currentTimeStamp);
        if (findIdx.has_value() == false)
        {
            solDebug << findIdx.error();
            _currentTimelineId = -1;
            return;
        }

        QModelIndex curIdx = _historyListModel->index(findIdx.value());
        if (const QAbstractProxyModel* proxy = qobject_cast<QAbstractProxyModel*>(_historyListView->model()))
        {
            curIdx = proxy->mapFromSource(curIdx);
        }

        _historyListView->setCurrentIndex(curIdx);
    });

    connect(_historyListView->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](const int min, const int max)
    {
        QScrollBar* scrollBar = _historyListView->verticalScrollBar();

        if (min < max)
        {
            const qreal newVal = _listScrollBarRatio * scrollBar->maximum();
            scrollBar->setValue(static_cast<int>(newVal));
        }
    });

    _historyListView->setItemDelegate(new HistoryListDelegate);
}

void HistoryWidget::exportSelectedHistoryData()
{
    const QModelIndex curIdx  = _historyListView->currentIndex();
    const int lastestRowIndex = curIdx.row();
    const std::expected<const HistoryCacheData*, QString> selectedTr = _historyListModel->getTranslateCache(lastestRowIndex);

    if (selectedTr.has_value() == false)
    {
        solDebug << selectedTr.error();
        return;
    }

    emit exportHistoryData(selectedTr.value());
}

void HistoryWidget::toggleTranslationText()
{
    const QModelIndex curIdx = _historyListView->currentIndex();
    if (curIdx.isValid() == false)
    {
        return;
    }

    // fix scrollbar
    const int prevVerticalScrollVal = _selectedTextEdit->verticalScrollBar()->value();

    // toggle
    _currentTextType = (_currentTextType == TextType::OriginText) ? TextType::TranslateText : TextType::OriginText;
    const Sol::HistoryItemRole currentTextTypeRole = (_currentTextType == TextType::OriginText) ? Sol::SourceFullTextRole : Sol::TargetFullTextRole;

    const QString nextText  = _historyListModel->data(curIdx, currentTextTypeRole).toString();
    const QString textStyle = _historyListModel->data(curIdx, Sol::TextStyleStringRole).toString();

    _selectedTextEdit->setFormattingText(nextText, Sol::qStrToEnum(textStyle, TextStyle::PlainText));

    _selectedTextEdit->verticalScrollBar()->setValue(prevVerticalScrollVal);
}
