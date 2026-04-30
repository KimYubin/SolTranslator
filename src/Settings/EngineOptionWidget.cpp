// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "SolTranslatorCore.h"
#include "ui_EngineOptionWidget.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"
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
    auto [aiOptionGroup, aiOptionVLay] = addNewOptionGroupBox(i18n(Tr::Ai_Options));

    // 온도 설정
    {
        const EngineId& engineId = EngineIds::OpenAI;
        const double defaultTemper = qobject_cast<IAiEngine>(EngineManager::getEngine(engineId)).getDefaultTemperature();
        
        SettingCard* openAiTemper = new SettingCard(new QDoubleSpinBox(this), aiOptionGroup);
        openAiTemper->setHeader(i18n(Tr::Temperature_Option).arg(i18n(Tr::OpenAI)));
        openAiTemper->setDescription(i18n(Tr::Default_Value_Hint).arg(defaultTemper));
        

        // openAiTemper->setDescription(i18n("값이 0에 가까울수록 고정된 답을 냅니다. 클수록 창의적이지만 부정확한 번역을 제공합니다."));
        QDoubleSpinBox* spinBox = openAiTemper->getContent<QDoubleSpinBox>();
        spinBox->setRange(0.0, 1.5);
        spinBox->setDecimals(2);
        spinBox->setSingleStep(0.1);
        spinBox->setValue(solConfig.Ai_Temperature(engineId));
        connect(spinBox, &QDoubleSpinBox::valueChanged, this, [engineId](const double inTemper)
        {
            solConfig.setAi_Temperature(engineId, inTemper);
        });


        aiOptionVLay->addWidget(openAiTemper, 0, Qt::AlignmentFlag::AlignTop);
    }

    initializeAfterCtor();
}

EngineOptionWidget::~EngineOptionWidget()
{
    delete ui;
}

void EngineOptionWidget::setEngineGroupUI()
{}



// ~======================
// EngineOption
EngineOption::EngineOption()
{
    setDisplayName(i18n(Tr::Translation_Engine));
    setIconPath("");
    setOptionWidgetCtor([]() { return new EngineOptionWidget(); });
    setPriority(1);
}

EngineOption::~EngineOption()
{
}

namespace
{
const EngineOption engineOption;
} // anonymous namespace
