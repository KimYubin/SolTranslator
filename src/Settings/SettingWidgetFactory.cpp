// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SettingWidgetFactory.h"

#include "Managers/ConfigManager.h"
#include "SubWidgets/CustomMenuTextEdit.h"
#include "SubWidgets/OptionGroupBox.h"
#include "SubWidgets/OptionLineEdit.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SolButton.h"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>

using Sol::i18n;

namespace
{
SettingCard* createBaseCard(QWidget* inContent
                          , QWidget* inParent
                          , const QString& inHeader
                          , const std::optional<QString>& inDescription
                          , const SettingCard::ContentPos contentPos = SettingCard::Right)
{
    SettingCard* resCard = new SettingCard(inContent, inParent, contentPos);
    resCard->setHeader(inHeader);
    if (inDescription.has_value())
    {
        resCard->setDescription(inDescription.value());
    }

    return resCard;
}
} // anonymous namespace

Expected<SettingCard*> CardFactory::createStringSaver(QWidget* inParent
                                                    , const OptionSpec& inOptSpec
                                                    , const QString& inCurrentVal
                                                    , Callback<void(const QString&)>&& inSetFunction)
{
    const StringSaver* stringSaverPtr = std::get_if<StringSaver>(&inOptSpec.defaultValue);
    if (stringSaverPtr == nullptr)
    {
        return makeUnexpected("OptionSpec.defaultValue is not StringSaver.");
    }

    const StringSaver& stringSaver = *stringSaverPtr;

    OptionLineEdit* optLineEdit = new OptionLineEdit(nullptr
                                                   , stringSaver.isUsedSaveButton
                                                   , inOptSpec.isSecretMode);

    const QString optDefaultStr = inOptSpec.getDefaultValue().toString();

    // The placeholder is a default value.
    optLineEdit->setDefaultTextAndText(optDefaultStr, inCurrentVal);
    optLineEdit->setSaveFunctor(std::move(inSetFunction));

    SettingCard* resCard = createBaseCard(optLineEdit, inParent, inOptSpec.headerName, inOptSpec.description, SettingCard::Down);

    return resCard;
}

Expected<SettingCard*> CardFactory::createDoubleSpin(QWidget* inParent
                                                   , const OptionSpec& inOptSpec
                                                   , const double inCurrentVal
                                                   , Callback<void(const double)>&& inSetFunction)
{
    const SpinData<double>* spinDataPtr = std::get_if<SpinData<double>>(&inOptSpec.defaultValue);
    if (spinDataPtr == nullptr)
    {
        return makeUnexpected("OptionSpec.defaultValue is not SpinData<double>.");
    }

    const SpinData<double>& spinData = *spinDataPtr;

    SettingCard* resCard = createBaseCard(new QDoubleSpinBox, inParent, inOptSpec.headerName, inOptSpec.description);

    QDoubleSpinBox* spinBox = resCard->getContent<QDoubleSpinBox>();
    spinBox->setRange(spinData.min, spinData.max);
    spinBox->setDecimals(spinData.decimals);
    spinBox->setSingleStep(spinData.singleStep);
    spinBox->setValue(inCurrentVal);

    connect(spinBox, &QDoubleSpinBox::valueChanged, std::move(inSetFunction));

    return resCard;
}


// ~===================
// OptionWidgetFactory

OptionGroupBox* OptionWidgetFactory::createOptionGroupBox(const QString& inGroupTitle
                                                        , QGridLayout* inParentLayout
                                                        , const int inRow
                                                        , const int inColumn
                                                        , const Qt::Alignment inAlignment)
{
    OptionGroupBox* groupBox = new OptionGroupBox(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inAlignment | Qt::AlignTop);

    return groupBox;
}

OptionGroupBox* OptionWidgetFactory::createOptionGroupBox(const QString& inGroupTitle
                                                        , QGridLayout* inParentLayout
                                                        , const int inRow
                                                        , const int inColumn
                                                        , const int inRowSpan
                                                        , const int inColumnSpan
                                                        , const Qt::Alignment inAlignment)
{
    OptionGroupBox* groupBox = new OptionGroupBox(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inRowSpan, inColumnSpan, inAlignment | Qt::AlignTop);

    return groupBox;
}
