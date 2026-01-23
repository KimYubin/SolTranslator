// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "LayoutTextEdit.h"

#include <QHBoxLayout>

LayoutTextEdit::LayoutTextEdit(QWidget* parent) : ResultTextEdit(parent)
{
    _bottomLayoutWidget = new QFrame(this);
    _bottomLayoutWidget->setObjectName("bottomWidget");

    _layout = new QHBoxLayout(_bottomLayoutWidget);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
}

LayoutTextEdit::~LayoutTextEdit()
{}

void LayoutTextEdit::resizeEvent(QResizeEvent* event)
{
    QTextEdit::resizeEvent(event);

    _bottomLayoutWidget->adjustSize();
    const QSize bottomSize = _bottomLayoutWidget->size();
    _bottomLayoutWidget->setGeometry(0, height() - bottomSize.height(), width(), bottomSize.height());
}

void LayoutTextEdit::addBottomWidget(QWidget* inWidget
                                   , const int inStretch
                                   , const Qt::Alignment inAlignment) const
{
    _layout->addWidget(inWidget, inStretch, inAlignment);
}

