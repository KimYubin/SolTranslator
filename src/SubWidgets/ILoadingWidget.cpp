// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ILoadingWidget.h"

#include "SolToolTip.h"

ILoadingWidget::ILoadingWidget(QWidget* parent)
    :QWidget(parent)
{}

void ILoadingWidget::setBubbleToolTip(const QString& inToolTip)
{
    SolToolTip::setToolTip(this, inToolTip);
}
