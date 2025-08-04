// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "LayoutTextEdit.h"

#include <QHBoxLayout>

LayoutTextEdit::LayoutTextEdit(QWidget* parent) : MenuTextEdit(parent)
{
    _bottomWidget = new QFrame(this);
    _bottomWidget->setObjectName("bottomWidget");

    _layout = new QHBoxLayout(_bottomWidget);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
}

LayoutTextEdit::~LayoutTextEdit() {}

void LayoutTextEdit::resizeEvent(QResizeEvent* event)
{
    QTextEdit::resizeEvent(event);

    _bottomWidget->adjustSize();
    const QSize bottomSize = _bottomWidget->size();
    _bottomWidget->setGeometry(0, height() - bottomSize.height(), width(), bottomSize.height());
}

bool LayoutTextEdit::focusNextPrevChild(bool next)
{
    QWidget* firstChild = _layout->parentWidget()->findChild<QWidget*>();
    // if (firstChild == nullptr)
    {
        return MenuTextEdit::focusNextPrevChild(next);
    }

    if (next)
    {
        if (firstChild->hasFocus() == false)
        {
            firstChild->setFocus();
            return true;
        }
        else if (firstChild->hasFocus())
        {
            if (nextInFocusChain())
            {
                nextInFocusChain()->setFocus();
                // return true;
            }
        }
    }
    else
    {
        if (firstChild->hasFocus())
        {
            setFocus();
            return true;
        }
    }

    return MenuTextEdit::focusNextPrevChild(next);
}
