// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SettingCard.h"

#include <QGridLayout>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>



SettingCard::SettingCard(QWidget* inContent, QWidget* parent, const ContentPos contentPos)
    : QFrame(parent)
    , _content(inContent)
    , _contentPos(contentPos)
{
    setObjectName("SettingCard");

    // layout
    _gridLayoutWidget = new QWidget(this);
    _gridLayoutWidget->setObjectName("gridLayoutWidget");

    _layout = new QGridLayout(_gridLayoutWidget);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setObjectName("layout");
    setLayout(_layout);

    // ordering header, content
    int headerCol  = 0;
    int contentCol = 1;
    if (_contentPos == ContentPos::Left)
    {
        headerCol  = 1;
        contentCol = 0;
    }
    _header = new QLabel(_gridLayoutWidget);
    _header->setObjectName("SettingHeaderText");
    _layout->addWidget(_header, 0, headerCol, 1, 1);

    _content->setParent(_gridLayoutWidget);
    _content->setObjectName("SettingContent");
    _layout->addWidget(_content, 0, contentCol, 1, 1);

    _layout->setColumnStretch(headerCol, 1);
}

SettingCard::~SettingCard()
{
}

void SettingCard::setHeader(const QString& inStr)
{
    _headerText = inStr;
    _header->setText(inStr);
}

void SettingCard::setDescription(const QString& inStr)
{
    _descriptionText = inStr;
    if (_description.has_value() == false)
    {
        _description = new QLabel(_gridLayoutWidget);
        _description.value()->setObjectName("SettingDescriptionText");

        int descCol     = 0;
        int descColSpan = 2;
        if (_contentPos == ContentPos::Left)
        {
            descCol     = 1;
            descColSpan = 1;
        }

        _layout->addWidget(_description.value(), 1, descCol, 1, descColSpan);
    }

    _description.value()->setText(inStr);
}
