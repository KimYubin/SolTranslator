// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryWidget.h"

#include <QGridLayout>
#include <QListView>
#include <QSplitter>

#include "SubWidgets/CustomMenuTextEdit.h"

HistoryWidget::HistoryWidget(QWidget* parent) : IFinWidget(parent)
{
    setupUI();
}

HistoryWidget::~HistoryWidget()
{
}

void HistoryWidget::setupUI()
{
    _mainLayout = new QGridLayout(this);
    _mainLayout->setSpacing(0);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    _splitter = new QSplitter(Qt::Horizontal);
    _splitter->setChildrenCollapsible(false);
    // splitter->setOpaqueResize(false);

    _mainLayout->addWidget(_splitter);

    _historyList = new QListView(_splitter);
    _historyList->setMinimumWidth(150);

    _splitter->addWidget(_historyList);

    _selectedDetail = new MenuTextEdit(_splitter);
    _selectedDetail->setMinimumWidth(150);

    _splitter->addWidget(_selectedDetail);
    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 2);

    // _splitter->setSizes({250, 500});
}
