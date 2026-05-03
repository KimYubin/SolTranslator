// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "SolTranslatorCore.h"
#include "ui_EngineOptionWidget.h"
#include "EngineUnits/IAiEngine.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/SettingCard.h"
#include "Types/EngineId.h"
#include "Utils/SolI18n.h"

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QString>
#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>

using Sol::i18n;

EngineOptionWidget::EngineOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::EngineOptionWidget)
{
    setObjectName("EngineOptionWidget");

    auto [engineGroup, engineVLay] = addNewOptionGroupBox(i18n(Tr::Translation_Engine_Settings));

    {
        ui->setupUi(engineGroup);
        engineVLay->addWidget(ui->gridLayoutWidget, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 번역 엔진 변경.
    ui->enginSelectCombo->setCurrentIndexChanged([this](const int inIdx)
    {
        const QString payload = ui->enginSelectCombo->itemData(inIdx).toString();
        const EngineId curEg  = EngineId{payload};
        const QString apiKey  = solConfig.apiKey(curEg);
        const int apiSize     = apiKey.size();

        QString phStr;
        if (apiSize > 15)
        {
            phStr = apiKey.sliced(0, 3).trimmed() + "..." + apiKey.last(4).trimmed();
        }
        else if (apiSize > 3)
        {
            phStr = QString(apiSize - 2, '*') + apiKey.last(2).trimmed();
        }
        else if (apiSize > 0)
        {
            phStr = QString(apiSize, '*');
        }

        ui->apiInputLine->setPlaceholderText(phStr);
    });

    // api 키 저장 및 적용
    connect(ui->apiKeySaveButton, &QPushButton::clicked, this, [this]()
    {
        const QString inputApiKey = ui->apiInputLine->text();
        if (inputApiKey.isEmpty())
        {
            return;
        }

        const QString payload = ui->enginSelectCombo->currentData().toString();
        const EngineId curEg  = EngineId{payload};

        solConfig.setApiKey(curEg, inputApiKey);
    });


    // AI 옵션
    const std::vector<QPointer<IAiEngine>> aiEngines = EngineManager::findEngines<IAiEngine>();
    for (const QPointer<IAiEngine>& engine : aiEngines)
    {
        setAiEngineUI(engine.data());
    }

    initializeAfterCtor();
}

EngineOptionWidget::~EngineOptionWidget()
{
    delete ui;
}

void EngineOptionWidget::setAiEngineUI(const IAiEngine* inEngine)
{
    const EngineId& engineId = inEngine->getEngineId();

    auto [optGroup, optVLay] = addNewOptionGroupBox(inEngine->getDisplayName() + " " + i18n(Tr::Options));

    const OptionMap& optionList = inEngine->getOptionDataList();
    for (const EngineOptionData& optData : optionList | std::views::values)
    {
        SettingCard* settingCard = nullptr;
        switch (optData.getOptionType())
        {
        case EngineOptionData::Type::None:
            break;
        case EngineOptionData::Type::Int:
            break;
        case EngineOptionData::Type::Double:
            break;
        case EngineOptionData::Type::Bool:
            break;
        case EngineOptionData::Type::SpinDataInt:
            break;
        case EngineOptionData::Type::SpinDataDouble:
        {
            settingCard = doubleSpinCard(optData, optGroup, engineId);
            break;
        }
        case EngineOptionData::Type::String:
            break;
        case EngineOptionData::Type::Combo:
            break;
        default: ;
        }

        if (settingCard)
        {
            optVLay->addWidget(settingCard, 0, Qt::AlignmentFlag::AlignTop);
        }
    }
}

SettingCard* EngineOptionWidget::baseSettingCard(const EngineOptionData& inOptData
                                               , QWidget* inParent)
{
    SettingCard* resCard = new SettingCard(new QDoubleSpinBox(this), inParent);
    resCard->setHeader(inOptData.headerName);
    if (inOptData.description.has_value())
    {
        resCard->setDescription(inOptData.description.value());
    }

    return resCard;
}

SettingCard* EngineOptionWidget::doubleSpinCard(const EngineOptionData& inOptData
                                              , QWidget* inParent
                                              , const EngineId& inEngineId)
{
    const SpinData<double>* spinDataPtr = std::get_if<SpinData<double>>(&inOptData.defaultValue);
    if (spinDataPtr == nullptr)
    {
        //
        return nullptr;
    }
    const SpinData<double>& spinData = *spinDataPtr;

    SettingCard* resCard = baseSettingCard(inOptData, inParent);

    QDoubleSpinBox* spinBox = resCard->getContent<QDoubleSpinBox>();
    spinBox->setRange(spinData.min, spinData.max);
    spinBox->setDecimals(spinData.decimals);
    spinBox->setSingleStep(spinData.singleStep);
    spinBox->setValue(solConfig.engineAttribute(inEngineId, inOptData.key).toDouble());

    connect(spinBox, &QDoubleSpinBox::valueChanged, this, [inEngineId, inKey = inOptData.key](const double inTemper)
    {
        solConfig.setEngineAttribute(inEngineId, inKey, inTemper);
    });

    return resCard;
}


// ~======================
// EngineOption
EngineOptionPage::EngineOptionPage()
{
    setDisplayName(i18n(Tr::Translation_Engine));
    setIconPath("");
    setOptionWidgetCtor([]() { return new EngineOptionWidget(); });
    setPriority(1);
}

EngineOptionPage::~EngineOptionPage()
{
}

namespace
{
const EngineOptionPage engineOptionPage;
} // anonymous namespace
