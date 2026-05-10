// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "OptionGroupBox.h"

#include <QRegularExpression>
#include <QVBoxLayout>

OptionGroupBox::OptionGroupBox(const QString& inGroupTitle, QWidget* parent)
    : QGroupBox(parent)
{
    QString objStr = inGroupTitle;
    objStr.remove(QRegularExpression("\\s"));
    objStr.remove(QRegularExpression("[^a-zA-Z0-9_-]"));

    setObjectName(objStr + "GroupBox");
    setAlignment(Qt::AlignmentFlag::AlignLeading | Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
    setFlat(true);
    setTitle(inGroupTitle);

    _vLayout = new QVBoxLayout(this);
    _vLayout->setObjectName(objStr + "VLayout");
    _vLayout->setContentsMargins(0, 0, 0, 0);

}

void OptionGroupBox::addChild(QWidget* inChild)
{
    _vLayout->addWidget(inChild, 0, Qt::AlignmentFlag::AlignTop);
}
