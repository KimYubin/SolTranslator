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
                                                    , const OptionData& inOptData
                                                    , const QString& inCurrentVal
                                                    , std::move_only_function<void(const QString&)>&& inSetFunction)
{
    const StringSaver* optDataPtr = std::get_if<StringSaver>(&inOptData.defaultValue);
    if (optDataPtr == nullptr)
    {
        return makeUnexpected("OptionData.defaultValue is not StringSaver.");
    }

    const StringSaver& optData = *optDataPtr;

    QWidget* layoutWidget    = new QWidget;
    OptionLineEdit* lineEdit = new OptionLineEdit(layoutWidget, inOptData.isSecretMode);
    QHBoxLayout* hLayout     = new QHBoxLayout(layoutWidget);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(lineEdit);

    const QString optDefaultStr = inOptData.getDefaultValue().toString();

    // The placeholder is a default value.
    lineEdit->setDefaultTextAndText(optDefaultStr, inCurrentVal);
    lineEdit->setSaveFunctor(std::move(inSetFunction));

    if (optData.isUsedSaveButton)
    {
        SolButton* saveButton = new SolButton(i18n(Tr::Save), layoutWidget);
        hLayout->addWidget(saveButton);
        connect(saveButton, &SolButton::clicked, lineEdit, &OptionLineEdit::saveText);
    }
    else
    {
        connect(lineEdit, &QLineEdit::textEdited, lineEdit, &OptionLineEdit::saveText);
    }

    SettingCard* resCard = createBaseCard(layoutWidget, inParent, inOptData.headerName, inOptData.description, SettingCard::Down);

    return resCard;
}

Expected<SettingCard*> CardFactory::createDoubleSpin(QWidget* inParent
                                                   , const OptionData& inOptData
                                                   , const double inCurrentVal
                                                   , std::move_only_function<void(const double)>&& inSetFunction)
{
    const SpinData<double>* spinDataPtr = std::get_if<SpinData<double>>(&inOptData.defaultValue);
    if (spinDataPtr == nullptr)
    {
        return makeUnexpected("OptionData.defaultValue is not SpinData<double>.");
    }

    const SpinData<double>& spinData = *spinDataPtr;

    SettingCard* resCard = createBaseCard(new QDoubleSpinBox, inParent, inOptData.headerName, inOptData.description);

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
