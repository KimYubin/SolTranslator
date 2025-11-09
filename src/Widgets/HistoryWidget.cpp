// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include <QGridLayout>
#include <QListView>
#include <QSplitter>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "FinHashQueue.h"
#include "FinTranslatorCore.h"
#include "HistoryModel.h"

#include "Managers/TranslateManager.h"

#include "SubWidgets/ResultTextEdit.h"


HistoryWidget::HistoryWidget(QWidget* parent) : IFinWidget(parent)
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
    QItemSelectionModel* selectionModel = _historyListView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, [this](QItemSelection selected, QItemSelection deselected)
    {
        const QModelIndexList slist = selected.indexes();
        const cache_queue& qlist    = finCore->translateManager()->getCacheQueue();

        auto contactIt = std::prev(qlist.end());
        for (int row = 0; row < std::min<int>(qlist.size(), slist.front().row()); ++row)
        {
            contactIt = std::prev(contactIt);
        }
        _selectedTextEdit->setFormattingText(contactIt->second, TextStyle::MarkDown);

        QTextCursor textCursor = _selectedTextEdit->textCursor();
        textCursor.setPosition(0);
        _selectedTextEdit->setTextCursor(textCursor);
    });

}

void HistoryWidget::addEntry(const QString& name, const QString& address)
{
    if (_historyListModel->getContacts().contains({name, address}))
        return;

    _historyListModel->insertRows(0, 1, QModelIndex());

    QModelIndex index;
    index = _historyListModel->index(0, 0, QModelIndex());
    _historyListModel->setData(index, name, Qt::EditRole);
    index = _historyListModel->index(0, 1, QModelIndex());
    _historyListModel->setData(index, address, Qt::EditRole);
    
}
