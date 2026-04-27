// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "DropdownMenu.h"

#include <QAbstractItemView>
#include <qevent.h>

DropdownMenu::DropdownMenu(QWidget* parent) : QComboBox(parent)
{
    // not WheelFocus
    setFocusPolicy(Qt::TabFocus);
}

void DropdownMenu::showPopup()
{
    QComboBox::showPopup();

    resetViewCurrentIndex();
}

void DropdownMenu::hidePopup()
{
    resetViewCurrentIndex();

    QComboBox::hidePopup();
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

void DropdownMenu::resetViewCurrentIndex()
{
    const QModelIndex curModelIdx
            = currentIndex() >= 0
                  ? model()->index(currentIndex(), modelColumn(), rootModelIndex())
                  : QModelIndex();

    view()->setCurrentIndex(curModelIdx);
}
