// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include <QGridLayout>
#include <QListView>
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

#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/ResultTextEdit.h"


HistoryWidget::HistoryWidget(QWidget* parent) : ISolWidget(parent)
{
    setupUI();
}

HistoryWidget::~HistoryWidget()
{
}

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

    _historyListView = new QListView(_splitter);
    _historyListView->setMinimumWidth(150);
    _historyListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _historyListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _historyListView->setUniformItemSizes(true);
    _historyListView->setLayoutMode(QListView::Batched);
    _historyListView->setBatchSize(10);


    _splitter->addWidget(_historyListView);

    _selectedTextEdit = new ResultTextEdit(_splitter);
    _selectedTextEdit->setObjectName("historySelectedDetail");
    _selectedTextEdit->setMinimumWidth(150);

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

    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected)
    {
        if (selected.empty())
        {
            return;
        }

        const int lastestRowIndex  = selected.indexes().back().row();
        const trDbInfo* selectedTr = _historyListModel->getTranslateText(lastestRowIndex);

        if (selectedTr == nullptr)
        {
            solDebug << "historyList out of range";
            return;
        }

        _selectedTextEdit->setFormattingText(selectedTr->_translateText, selectedTr->_textStyle);

        QTextCursor textCursor = _selectedTextEdit->textCursor();
        textCursor.setPosition(0);
        _selectedTextEdit->setTextCursor(textCursor);
    });


    // 모델 리셋 시, 스크롤 위치 유지
    connect(_historyListModel, &QAbstractItemModel::modelAboutToBeReset, _historyListView, [this]()
    {
        const QScrollBar* scrollBar = _historyListView->verticalScrollBar();
        const qreal currentScroll   = scrollBar->value();
        const qreal maxScroll       = scrollBar->maximum();
        _listScrollBarRatio         = (maxScroll > 0) ? (currentScroll / maxScroll) : 0.0;
    });

    connect(_historyListView->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](const int min, const int max)
    {
        QScrollBar* scrollBar = _historyListView->verticalScrollBar();

        if ((min < max) && scrollBar->value() == 0)
        {
            const qreal newVal = _listScrollBarRatio * scrollBar->maximum();
            scrollBar->setValue(static_cast<int>(newVal));
        }
    });
    
    _historyListView->setItemDelegate(new HistoryListDelegate);
}
