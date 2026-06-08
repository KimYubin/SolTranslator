// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "DropdownMenu.h"

#include "SolToolTip.h"
#include "Managers/ConfigManager.h"

#include <QAbstractItemView>
#include <QShortcut>
#include <qevent.h>

DropdownMenu::DropdownMenu(QWidget* parent) : QComboBox(parent)
{
    // not WheelFocus
    setFocusPolicy(Qt::TabFocus);
}

QSize DropdownMenu::sizeHint() const
{
    // Padding for icon
    return QComboBox::sizeHint() + QSize(15, 0);
}

QSize DropdownMenu::minimumSizeHint() const
{
    // Padding for icon
    return QComboBox::minimumSizeHint() + QSize(15, 0);
}

void DropdownMenu::showPopup()
{
    QComboBox::showPopup();

    resetViewCurrentIndex();
}

void DropdownMenu::hidePopup()
{
    QComboBox::hidePopup();
}

void DropdownMenu::setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey)
{
    _toolTip = inToolTip;

    if (_shortcut.isNull())
    {
        _shortcut = new QShortcut(this);
    }
    _shortcut->setKey(inKey);

    connect(_shortcut, &QShortcut::activated, this, &DropdownMenu::showPopup);

    SolToolTip::setBubbleToolTip(this, Sol::toolTipShortcut(_toolTip, inKey));
}

void DropdownMenu::setToolTipAction(const QString& inToolTip, const Action inAction)
{
    setToolTipShortcut(inToolTip, solConfig.shortcut(inAction));
}

void DropdownMenu::changeShortcut(const QKeySequence& inKey)
{
    setToolTipShortcut(_toolTip, inKey);
}

void DropdownMenu::setAction(const Action inAction)
{
    changeShortcut(solConfig.shortcut(inAction));
}

void DropdownMenu::setBubbleToolTip(const QString& inToolTip)
{
    setToolTipShortcut(inToolTip, _shortcut.isNull() ? QKeySequence() : _shortcut->key());
}

void DropdownMenu::wheelEvent(QWheelEvent* event)
{
    if (hasFocus())
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
