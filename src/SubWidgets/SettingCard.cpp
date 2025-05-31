//
// Created by YubinKim on 25/05/27 화.
//

#include "SettingCard.h"

#include <QGridLayout>
#include <QLabel>


SettingCard::SettingCard(QWidget* inInteractionWidget, QWidget* parent) : QWidget(parent)
{
    _gridLayoutWidget = new QWidget(this);
    _gridLayoutWidget->setObjectName("gridLayoutWidget");

    _layout = new QGridLayout(_gridLayoutWidget);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setObjectName("layout");
    setLayout(_layout);

    _header = new QLabel(_gridLayoutWidget);
    _layout->addWidget(_header, 0, 0, 1, 1);

    _interactionWidget = inInteractionWidget;
    _interactionWidget->setParent(_gridLayoutWidget);
    _layout->addWidget(_interactionWidget, 0, 1, 1, 1);

    _layout->setColumnStretch(0, 1);
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
        _layout->addWidget(_description.value(), 1, 0, 1, 2);
    }

    _description.value()->setText(inStr);
}
