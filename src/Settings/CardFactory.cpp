// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "CardFactory.h"

#include "SubWidgets/SettingCard.h"

#include <QDoubleSpinBox>

SettingCard* CardFactory::createBaseCard(QWidget* inContent
                                       , QWidget* inParent
                                       , const QString& inHeader
                                       , const std::optional<QString>& inDescription)
{
    SettingCard* resCard = new SettingCard(inContent, inParent);
    resCard->setHeader(inHeader);
    if (inDescription.has_value())
    {
        resCard->setDescription(inDescription.value());
    }

    return resCard;
}

std::expected<SettingCard*, QString> CardFactory::createDoubleSpin(QWidget* inParent
                                                                 , const OptionData& inOptData
                                                                 , const double inCurrentVal)
{
    const SpinData<double>* spinDataPtr = std::get_if<SpinData<double>>(&inOptData.defaultValue);
    if (spinDataPtr == nullptr)
    {
        return std::unexpected{"OptionData.defaultValue is not SpinData<double>."};
    }

    const SpinData<double>& spinData = *spinDataPtr;

    SettingCard* resCard = createBaseCard(new QDoubleSpinBox, inParent, inOptData.headerName, inOptData.description);

    QDoubleSpinBox* spinBox = resCard->getContent<QDoubleSpinBox>();
    spinBox->setRange(spinData.min, spinData.max);
    spinBox->setDecimals(spinData.decimals);
    spinBox->setSingleStep(spinData.singleStep);
    spinBox->setValue(inCurrentVal);

    return resCard;
}
