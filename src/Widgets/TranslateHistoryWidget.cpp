// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateHistoryWidget.h"

#include <QGridLayout>

TranslateHistoryWidget::TranslateHistoryWidget(QWidget* parent) : IFinWidget(parent)
{
    setupUI();
}

TranslateHistoryWidget::~TranslateHistoryWidget()
{
}

void TranslateHistoryWidget::setupUI()
{
    _mainLayout = new QGridLayout(this);
    _mainLayout->setSpacing(0);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setContentsMargins(0, 0, 0, 0);
}
