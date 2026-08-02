// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ISolWidget.h"

ISolWidget::ISolWidget(QWidget* inParent, const Qt::WindowFlags inFlags)
    : QWidget(inParent, inFlags)
{}

ISolWidget::~ISolWidget()
{}

void ISolWidget::showRaiseUp()
{
    if (isMinimized())
    {
        showNormal();
    }
    if (isHidden())
    {
        show();
    }

    raise();
    activateWindow();
}
