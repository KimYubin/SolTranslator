// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include <QString>
#include <QDoubleSpinBox>
#include <QGroupBox>

#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>

#include "Managers/ConfigManager.h"
#include "FinTranslatorCore.h"
#include "Widgets/FinTranslatorMainWidget.h"
#include "SubWidgets/DropdownMenu.h"

#include "ui_EngineOptionWidget.h"

#include "SubWidgets/SettingCard.h"

EngineOptionWidget::EngineOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::EngineOptionWidget)
{
    setObjectName("EngineOptionWidget");

    auto [engineGroup, engineVLay] = addNewOptionGroupBox(tr("번역 엔진 설정"));

    {
        ui->setupUi(engineGroup);
        engineVLay->addWidget(ui->gridLayoutWidget, 0, Qt::AlignmentFlag::AlignTop);
    }
    

    ui->enginSelectCombo->setEditable(false);

    // 엔진 선택 초기화
    for (EngineType eg = EngineType::Default; eg != EngineType::Size; eg = static_cast<EngineType>(static_cast<int>(eg) + 1))
    {
        ui->enginSelectCombo->addItem(EngineName::getName(eg), static_cast<int>(eg));
    }

    // 번역 엔진 변경.
    connect(ui->enginSelectCombo, &QComboBox::currentIndexChanged, this, [this](const int inIdx)
    {
        const int payload    = ui->enginSelectCombo->itemData(inIdx).toInt();
        const EngineType eg  = static_cast<EngineType>(payload);
        const QString apiKey = finConfig.getAPIKey(eg);
        const int apiSize    = apiKey.size();

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
        const int payload   = ui->enginSelectCombo->currentData().toInt();
        const EngineType eg = static_cast<EngineType>(payload);

        finConfig.setAPIKey(eg, inputApiKey);
    });


    ui->enginSelectCombo->setCurrentIndex(static_cast<int>(finConfig.getCurrentEngineType()));


    // AI 옵션
    auto [aiOptionGroup, aiOptionVLay] = addNewOptionGroupBox(tr("AI 옵션"));

    // 온도 설정
    {
        SettingCard* openAiTemper = new SettingCard(new QDoubleSpinBox(this), aiOptionGroup);
        openAiTemper->setHeader(tr("OpenAI 온도 설정"));
        openAiTemper->setDescription(tr("(기본값: 0.5)"));

        // openAiTemper->setDescription(tr("값이 0에 가까울수록 고정된 답을 냅니다. 클수록 창의적이지만 부정확한 번역을 제공합니다."));
        QDoubleSpinBox* spinBox = openAiTemper->getContent<QDoubleSpinBox>();
        spinBox->setRange(0.0, 1.5);
        spinBox->setDecimals(2);
        spinBox->setSingleStep(0.1);
        spinBox->setValue(finConfig.getOpenAI_Temperature());
        connect(spinBox, &QDoubleSpinBox::valueChanged, this, [](const double inTemper)
        {
            finConfig.setOpenAI_Temperature(inTemper);
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
{
}



// ~======================
// EngineOption 
EngineOption::EngineOption()
{
    setDisplayName(tr("번역 엔진"));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new EngineOptionWidget(); });
    setPriority(OptionPriority::EngineOption);
}

EngineOption::~EngineOption()
{
}

const EngineOption engineOption;
