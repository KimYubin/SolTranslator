// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "DropdownMenu.h"

#include <qevent.h>

DropdownMenu::DropdownMenu(QWidget* parent): QComboBox(parent)
{
    // not WheelFocus
    setFocusPolicy(Qt::TabFocus);
}

void DropdownMenu::wheelEvent(QWheelEvent* event)
{
    if (this->hasFocus())
    {
        QComboBox::wheelEvent(event);
    }
    else
    {
        // 포커스 없으면 wheel 이벤트 무시
        event->ignore();
    }
}
