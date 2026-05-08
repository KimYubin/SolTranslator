// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SettingWidgetFactory.h"

#include "Managers/ConfigManager.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SolButton.h"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
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

    QWidget* layoutWidget = new QWidget;
    QLineEdit* lineEdit   = new QLineEdit(layoutWidget);
    QHBoxLayout* hLayout  = new QHBoxLayout(layoutWidget);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(lineEdit);

    QString phText = inCurrentVal;
    if (inOptData.isSecretMode)
    {
        lineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        if (inCurrentVal.size() > 15)
        {
            phText = inCurrentVal.sliced(0, 3).trimmed() + "..." + inCurrentVal.last(4).trimmed();
        }
        else if (inCurrentVal.size() > 0)
        {
            phText = QString{"*******"};
        }
    }
    lineEdit->setPlaceholderText(phText);

    auto setupFunctor = [lineEdit, setFunc = std::move(inSetFunction)]() mutable
    {
        const QString inputText = lineEdit->text();
        if (inputText.isEmpty())
        {
            return;
        }

        setFunc(inputText);
    };

    if (optData.isUsedSaveButton)
    {
        SolButton* saveButton = new SolButton(layoutWidget);
        saveButton->setText(i18n(Tr::Save));
        hLayout->addWidget(saveButton);
        connect(saveButton, &SolButton::clicked, lineEdit, std::move(setupFunctor));
    }
    else
    {
        connect(lineEdit, &QLineEdit::textEdited, std::move(setupFunctor));
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
