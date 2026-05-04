// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "CardFactory.h"
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
    for (const OptionData& optData : optionList | std::views::values)
    {
        SettingCard* settingCard = nullptr;
        switch (optData.getOptionType())
        {
        case OptionData::Type::None:
            break;
        case OptionData::Type::Int:
            break;
        case OptionData::Type::Double:
            break;
        case OptionData::Type::Bool:
            break;
        case OptionData::Type::SpinDataInt:
            break;
        case OptionData::Type::SpinDataDouble:
        {
            settingCard = doubleSpinCard(optGroup, engineId, optData);
            break;
        }
        case OptionData::Type::String:
            break;
        case OptionData::Type::Combo:
            break;
        default: ;
        }

        if (settingCard)
        {
            optVLay->addWidget(settingCard, 0, Qt::AlignmentFlag::AlignTop);
        }
    }
}

SettingCard* EngineOptionWidget::doubleSpinCard(QWidget* inParent
                                              , const EngineId& inEngineId
                                              , const OptionData& inOptData)
{
    const double curValue = solConfig.engineAttribute(inEngineId, inOptData.key).toDouble();
    const std::expected<SettingCard*, QString> resCardExp = CardFactory::createDoubleSpin(inParent, inOptData, curValue);
    if (resCardExp.has_value() == false)
    {
        // todo: 적절한 오류처리
        return nullptr;
    }
    SettingCard* resCard = resCardExp.value();

    const QDoubleSpinBox* spinBox = resCard->getContent<QDoubleSpinBox>();
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
