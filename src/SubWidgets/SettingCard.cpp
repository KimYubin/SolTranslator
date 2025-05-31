//
// Created by YubinKim on 25/05/27 화.
//

#include "SettingCard.h"

#include <QGridLayout>
#include <QLabel>

#include "SwitchButton.h"

SettingCard::SettingCard(QWidget* parent) : QWidget(parent)
{
    _gridLayoutWidget = new QWidget(this);
    _gridLayoutWidget->setObjectName("gridLayoutWidget");

    _layout = new QGridLayout(_gridLayoutWidget);
    _layout->setContentsMargins(0, 0, 0, 0);
    // _layout->setSpacing(8);
    _layout->setObjectName("layout");
    setLayout(_layout);

    _header = new QLabel(_gridLayoutWidget);
    _layout->addWidget(_header, 0, 0, 1, 1);

    _switchButton = new SwitchButton(_gridLayoutWidget);
    _layout->addWidget(_switchButton, 0, 1, 1, 1);

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

void SettingCard::connectCheckStateChange(QObject* inContext, std::function<void(Qt::CheckState inCheckState)>&& callback)
{
    connect(_switchButton
          , &QCheckBox::checkStateChanged
          , this
          , std::forward<std::function<void(Qt::CheckState inCheckState)>>(callback));
}

void SettingCard::setCheckable(const bool inCheckable) 
{
    _switchButton->setCheckable(inCheckable);
}

bool SettingCard::isCheckable() const
{
    return _switchButton->isCheckable();
}

void SettingCard::setCheck(const bool inCheck)
{
    _switchButton->setChecked(inCheck);
}

bool SettingCard::isCheck() const
{
    return _switchButton->isChecked();
}
