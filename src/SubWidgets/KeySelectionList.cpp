// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "KeySelectionList.h"

#include <QKeyEvent>

KeySelectionList::KeySelectionList(QWidget* inParent) : QListWidget(inParent)
{}

void KeySelectionList::keyPressEvent(QKeyEvent* inEvent)
{
    if (inEvent->key() == Qt::Key_Return || inEvent->key() == Qt::Key_Enter || inEvent->key() == Qt::Key_Space)
    {
        if (QListWidgetItem* item = currentItem())
        {
            emit itemKeyPressed(item);
        }
        return;
    }

    QListWidget::keyPressEvent(inEvent);
}
