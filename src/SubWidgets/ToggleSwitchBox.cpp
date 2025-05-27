//
// Created by YubinKim on 25/05/27 화.
//

#include "ToggleSwitchBox.h"

#include <QGridLayout>
#include <QLabel>

#include "SwitchButton.h"

ToggleSwitchBox::ToggleSwitchBox(QWidget* parent) : QWidget(parent)
{
    auto* gridLayoutWidget = new QWidget(this);
    gridLayoutWidget->setObjectName("gridLayoutWidget");
    gridLayoutWidget->setGeometry(QRect(100, 50, 521, 281));
    _layout = new QGridLayout(gridLayoutWidget);    
    _layout->setSpacing(8);
    _layout->setObjectName("layout");
    _layout->setContentsMargins(8, 8, 8, 8);
    setLayout(_layout);
    
    _header      = new QLabel(gridLayoutWidget);
    _switchButton = new SwitchButton(gridLayoutWidget);

    _description = new QLabel(gridLayoutWidget);

    
    _layout->addWidget(_header, 0, 0, 1, 1);
    _layout->addWidget(_switchButton, 0, 1, 1, 1);
    _layout->setColumnStretch(1, 1);

    _layout->addWidget(_description, 1, 0, 1, 2);
}

ToggleSwitchBox::~ToggleSwitchBox()
{
}

void ToggleSwitchBox::setHeader(const QString& inStr)
{
    _headerText = inStr;
    _header->setText(inStr);
}

void ToggleSwitchBox::setDescription(const QString& inStr)
{
    _descriptionText = inStr;
    _description->setText(inStr);
}
