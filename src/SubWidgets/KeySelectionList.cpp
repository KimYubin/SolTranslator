// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "KeySelectionList.h"

#include <QKeyEvent>

KeySelectionList::KeySelectionList(QWidget* parent): QListWidget(parent)
{
}

void KeySelectionList::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space)
    {
        QListWidgetItem* item = currentItem();
        if (item)
        {
            emit itemKeyPressed(item);
        }
        return;
    }

    QListWidget::keyPressEvent(event);
}
