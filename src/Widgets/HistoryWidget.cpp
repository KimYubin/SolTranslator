// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include <QClipboard>

#include <expected>
#include <QGridLayout>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QTimer>

#include "HistoryListDelegate.h"
#include "SolHashQueue.h"
#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "HistoryModel.h"
#include "SolMainWidget.h"

#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/LayoutTextEdit.h"
#include "SubWidgets/SolToast.h"
#include "SubWidgets/SolWidgetFactory.h"


HistoryListView::HistoryListView(QWidget* parent) : QListView(parent)
{}

HistoryListView::~HistoryListView()
{}

QColor HistoryListView::getItemColor(const sol::ItemColorRole inColorRole) const
{
    switch (inColorRole)
    {
    case sol::itemTextColorRole:          return _itemTextColor;
    case sol::itemSelectionTextColorRole: return _itemSelectionTextColor;
    case sol::itemHoverTextColorRole:     return _itemHoverTextColor;
    case sol::itemDisableColorRole:       return _itemDisableColor;
    default: ;
    }
    return QColor();
}

HistoryWidget::HistoryWidget(QWidget* parent) : ISolWidget(parent)
{
    setupUI();

    _currentTextRole = sol::TargetFullTextRole;
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
    QPushButton* trCopy = SolWidgetFactory::createCopyButton(_selectedTextEdit, [this]()
    {
        const QModelIndex curIdx = _historyListView->currentIndex();

        return _historyListModel->data(curIdx, sol::TargetFullTextRole).toString();
    });
    _selectedTextEdit->addBottomWidget(trCopy, 0, Qt::AlignLeft);

    // 원문/번역 토글
    {
        QPushButton* textToggleButton = _selectedTextEdit->addBottomButton(QIcon(":/img/swap_text_img")
                                                                         , Qt::TabFocus
                                                                         , tr("원문/번역 토글(<u>T<\\u>)")
                                                                         , 0
                                                                         , Qt::AlignLeft);

        textToggleButton->setShortcut(Qt::Key_T);
        connect(textToggleButton, &QPushButton::clicked, this, [this]() { toggleTranslationText(); });
    }

    // 기록 삭제
    {
        QPushButton* deleteButton = _selectedTextEdit->addBottomButton(QIcon(":/img/delete_img")
                                                                     , Qt::TabFocus
                                                                     , tr("번역 삭제")
                                                                     , 1
                                                                     , Qt::AlignRight);

        connect(deleteButton, &QPushButton::clicked, this, [this]()
        {
            const QModelIndex curIdx = _historyListView->currentIndex();
            const qlonglong dbId     = _historyListModel->data(curIdx, sol::DbIdRole).toLongLong();
            solCore->historyManager()->deleteHistory(dbId);
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
        _currentTextRole   = sol::TargetFullTextRole;

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

        const int findIdx = solCore->historyManager()->findModelIdxFromTimelineId(_currentTimelineId, _currentTimeStamp);
        if (findIdx < 0)
        {
            _currentTimelineId = -1;
            return;
        }

        QModelIndex curIdx = _historyListModel->index(findIdx);
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
    _currentTextRole = (_currentTextRole == sol::TargetFullTextRole) ? sol::SourceFullTextRole : sol::TargetFullTextRole;

    const QString nextText  = _historyListModel->data(curIdx, _currentTextRole).toString();
    const QString textStyle = _historyListModel->data(curIdx, sol::TextStyleStringRole).toString();

    _selectedTextEdit->setFormattingText(nextText, sol::qStrToEnum(textStyle, TextStyle::PlainText));

    _selectedTextEdit->verticalScrollBar()->setValue(prevVerticalScrollVal);
}
