// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "LayoutTextEdit.h"

#include "SolButton.h"
#include "SolToolTip.h"

#include <QAbstractButton>
#include <QHBoxLayout>

namespace Sol
{
LayoutTextEdit::LayoutTextEdit(QWidget* inParent) : ResultTextEdit(inParent)
{
    _bottomLayoutWidget = new QFrame(this);
    _bottomLayoutWidget->setObjectName("bottomWidget");

    _layout = new QHBoxLayout(_bottomLayoutWidget);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
}

LayoutTextEdit::~LayoutTextEdit()
{}

void LayoutTextEdit::resizeEvent(QResizeEvent* inEvent)
{
    QTextEdit::resizeEvent(inEvent);

    _bottomLayoutWidget->adjustSize();
    const QSize bottomSize = _bottomLayoutWidget->size();
    _bottomLayoutWidget->setGeometry(0, height() - bottomSize.height(), width(), bottomSize.height());
}

void LayoutTextEdit::addBottomWidget(QWidget* inWidget
                                   , const int inStretch
                                   , const Qt::Alignment inAlignment)
{
    _layout->addWidget(inWidget, inStretch, inAlignment);
}

SolButton* LayoutTextEdit::addBottomButton(const QIcon& inIcon
                                         , const Qt::FocusPolicy inPolicy
                                         , const QString& inToolTip
                                         , const QKeySequence& inKey
                                         , const int inStretch
                                         , const Qt::Alignment inAlignment)
{
    SolButton* newButton = new SolButton(this);
    newButton->setIcon(inIcon);
    newButton->setFocusPolicy(inPolicy);
    newButton->setToolTipShortcut(inToolTip, inKey);

    addBottomWidget(newButton, inStretch, inAlignment);

    return newButton;
}
} // namespace Sol
