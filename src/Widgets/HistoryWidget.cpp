// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include "HistoryListDelegate.h"
#include "HistoryListView.h"
#include "HistoryModel.h"
#include "PopupTranslateWidget.h"
#include "SolMainWidget.h"
#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "SubWidgets/LayoutTextEdit.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolWidgetFactory.h"
#include "Utils/SolDebug.h"
#include "Utils/SolI18n.h"

#include <QGridLayout>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QTimer>



namespace Sol
{
HistoryWidget::HistoryWidget(QWidget* inParent) : ISolWidget(inParent)
{
    setupUI();

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


    // ~=============
    // buttons

    // 복사 버튼
    SolButton* trCopy = SolWidgetFactory::createCopyButton(_selectedTextEdit, [this]()->QString
    {
        const QModelIndex curIdx = _historyListView->currentIndex();
        if (curIdx.isValid() == false)
        {
            return {};
        }

        ExpectedHistory historyData = getHistoryData(curIdx);
        if (!historyData)
        {
            solDebug << historyData.error();
            return {};
        }

        return historyData.value()->getText(_currentTextCategory);
    });
    _selectedTextEdit->addBottomWidget(trCopy, 0, Qt::AlignLeft);


    // 원문/번역 토글
    SolButton* toggleButton = SolWidgetFactory::createToggleButton(this, [this]() { toggleTranslationText(); });
    _selectedTextEdit->addBottomWidget(toggleButton, 0, Qt::AlignLeft);


    // 재번역 버튼
    SolButton* reTranslateBtn = SolWidgetFactory::createReTranslateButton(this, [this]() { reTranslate(); });
    _selectedTextEdit->addBottomWidget(reTranslateBtn, 0, Qt::AlignLeft);


    // 팝업
    SolButton* popupBtn = SolWidgetFactory::createViewInPopup(this, [this](){viewPopup();});
    _selectedTextEdit->addBottomWidget(popupBtn, 0, Qt::AlignLeft);


    // 기록 삭제 버튼
    SolButton* deleteButton = SolWidgetFactory::createDeleteTranslation(this, [this]()
    {
        _currentTimelineId = -1;
        const QModelIndex curIdx = _historyListView->currentIndex();
        if (curIdx.isValid() == false)
        {
            return;
        }
        ExpectedHistory historyData = getHistoryData(curIdx);
        if (!historyData)
        {
            solDebug << historyData.error();
            return;
        }

        const qlonglong dbId = historyData.value()->getDbId();
        solCore->manager<HistoryManager>()->asyncDeleteHistory(dbId);
        _selectedTextEdit->setText("");
    });
    _selectedTextEdit->addBottomWidget(deleteButton, 1, Qt::AlignRight);


    _splitter->addWidget(_selectedTextEdit);
    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 2);
    // _splitter->setSizes({250, 500});


    // ~===========
    // history list model
    _historyListModel = new HistoryModel(this);

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_historyListModel);
    proxyModel->setFilterKeyColumn(0);

    _historyListView->setModel(proxyModel);
    _historyListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _historyListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _historyListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // select item
    const QItemSelectionModel* selectionModel = _historyListView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& inCurrent, const QModelIndex&)
    {
        if (inCurrent.isValid() == false)
        {
            return;
        }
        ExpectedHistory historyData = getHistoryData(inCurrent);
        if (!historyData)
        {
            solDebug << historyData.error();
            return;
        }

        // only db update
        const qint64 newCurrentTimelineId = historyData.value()->getTimelineId();
        if (newCurrentTimelineId == _currentTimelineId)
        {
            return;
        }
        _currentTimelineId   = newCurrentTimelineId;
        _currentTimeStamp    = historyData.value()->getTimeStamp();
        _currentTextCategory = TextCategory::TargetText;

        const QString curText     = historyData.value()->getTargetText();
        const TextStyle textStyle = historyData.value()->getTextStyle();
        _selectedTextEdit->setFormattingText(curText, textStyle);

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

        const Expected<int> findIdx = solCore->manager<HistoryManager>()->findModelIdxFromTimelineId(_currentTimelineId, _currentTimeStamp);
        if (!findIdx)
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

    connect(_historyListView->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](const int inMin, const int inMax)
    {
        QScrollBar* scrollBar = _historyListView->verticalScrollBar();

        if (inMin < inMax)
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
    if (curIdx.isValid() == false)
    {
        return;
    }
    ExpectedHistory historyData = getHistoryData(curIdx);
    if (!historyData)
    {
        solDebug << historyData.error();
        return;
    }

    emit exportHistoryData(historyData.value());
}

void HistoryWidget::toggleTranslationText()
{
    const QModelIndex curIdx = _historyListView->currentIndex();
    if (curIdx.isValid() == false)
    {
        return;
    }
    ExpectedHistory historyData = getHistoryData(curIdx);
    if (!historyData)
    {
        solDebug << historyData.error();
        return;
    }

    // toggle
    _currentTextCategory = (_currentTextCategory == TextCategory::SourceText) ? TextCategory::TargetText : TextCategory::SourceText;

    const QString nextText    = historyData.value()->getText(_currentTextCategory);
    const TextStyle textStyle = historyData.value()->getTextStyle();

    // fix scrollbar
    const int prevVerticalScrollVal = _selectedTextEdit->verticalScrollBar()->value();
    _selectedTextEdit->setFormattingText(nextText, textStyle);
    _selectedTextEdit->verticalScrollBar()->setValue(prevVerticalScrollVal);
}

void HistoryWidget::reTranslate() const
{
    const QModelIndex curIdx = _historyListView->currentIndex();
    if (curIdx.isValid() == false)
    {
        return;
    }
    ExpectedHistory historyData = getHistoryData(curIdx);
    if (!historyData)
    {
        solDebug << historyData.error();
        return;
    }

    const QString sourceText  = historyData.value()->getSourceText();
    const TextStyle textStyle = historyData.value()->getTextStyle();

    solCore->manager<TranslateManager>()->translateAtPopup(sourceText, textStyle, true);
}

void HistoryWidget::viewPopup() const
{
    const QModelIndex curIdx = _historyListView->currentIndex();
    if (curIdx.isValid() == false)
    {
        return;
    }
    ExpectedHistory historyData = getHistoryData(curIdx);
    if (!historyData)
    {
        solDebug << historyData.error();
        return;
    }

    const QString sourceText  = historyData.value()->getSourceText();
    const QString targetText  = historyData.value()->getTargetText();
    const TextStyle textStyle = historyData.value()->getTextStyle();

    PopupTranslateWidget* popupWidget = new PopupTranslateWidget();
    popupWidget->viewTranslationText(sourceText, targetText, textStyle);
}

HistoryWidget::ExpectedHistory HistoryWidget::getHistoryData(const QModelIndex& inCurIdx) const
{
    return _historyListModel->getHistoryCacheData(inCurIdx);
}
} // namespace Sol
