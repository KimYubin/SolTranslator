// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SettingWidgetFactory.h"

#include "SubWidgets/SettingCard.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>

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

Expected<SettingCard*> CardFactory::createDoubleSpin(QWidget* inParent
                                                   , const OptionData& inOptData
                                                   , const double inCurrentVal)
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

    return resCard;
}


// ~===================
// OptionWidgetFactory
GroupLayout OptionWidgetFactory::generateGroupBoxLayout(const QString& inGroupTitle)
{
    QString objStr = inGroupTitle;
    objStr.remove(QRegularExpression("\\s"));
    objStr.remove(QRegularExpression("[^a-zA-Z0-9_-]"));

    QGroupBox* groupBox = new QGroupBox();
    groupBox->setObjectName(objStr + "GroupBox");
    groupBox->setAlignment(Qt::AlignmentFlag::AlignLeading | Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
    groupBox->setFlat(true);
    groupBox->setTitle(inGroupTitle);

    QVBoxLayout* vLayout = new QVBoxLayout(groupBox);
    vLayout->setObjectName(objStr + "VLayout");
    vLayout->setContentsMargins(0, 0, 0, 0);

    return {groupBox, vLayout};
}

GroupLayout OptionWidgetFactory::createOptionGroupBox(const QString& inGroupTitle
                                                    , QGridLayout* inParentLayout
                                                    , const int inRow
                                                    , const int inColumn
                                                    , const Qt::Alignment inAlignment)
{
    auto [groupBox, vLayout] = generateGroupBoxLayout(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inAlignment | Qt::AlignTop);

    return {groupBox, vLayout};
}

GroupLayout OptionWidgetFactory::createOptionGroupBox(const QString& inGroupTitle
                                                    , QGridLayout* inParentLayout
                                                    , const int inRow
                                                    , const int inColumn
                                                    , const int inRowSpan
                                                    , const int inColumnSpan
                                                    , const Qt::Alignment inAlignment)
{
    auto [groupBox, vLayout] = generateGroupBoxLayout(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inRowSpan, inColumnSpan, inAlignment | Qt::AlignTop);

    return {groupBox, vLayout};
}
