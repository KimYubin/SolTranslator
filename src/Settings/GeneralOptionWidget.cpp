// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GeneralOptionWidget.h"

#include <QGroupBox>
#include <QPushButton>

#include "FinTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SwitchButton.h"
#include "Widgets/FinTranslatorMainWidget.h"

#include "ui_GeneralOptionWidget.h"

#include "Managers/StyleManger.h"

#include "SubWidgets/DropdownMenu.h"


GeneralOptionWidget::GeneralOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::GeneralOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    ui->mainLayout->setContentsMargins(0, 0, 0, 0);

    auto [shapeBehaviorGroup, shapeBehaviorVLay] = newOptionGroupBox(tr("모양 및 동작"), ui->mainLayout, ui->mainLayout->rowCount(), 0);

    // 시작시 실행
    {
        SettingCard* startRunCard = new SettingCard(new SwitchButton(finConfig.getStartRun()), shapeBehaviorGroup);
        startRunCard->setHeader(tr("시작 시 실행"));
        startRunCard->setDescription(tr("시스템 시작 시 Fin번역기가 자동으로 실행됩니다."));
        SwitchButton* startRunSwitch = startRunCard->getContent<SwitchButton>();
        connect(startRunSwitch, &QCheckBox::checkStateChanged, this, [](Qt::CheckState inState)
        {
            finConfig.setStartRun(inState == Qt::CheckState::Checked);
        });
        shapeBehaviorVLay->addWidget(startRunCard, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 팝업번역 도착언어 선택
    {
        SettingCard* selectTargetLang = new SettingCard(new DropdownMenu, shapeBehaviorGroup);
        selectTargetLang->setHeader(tr("목표 언어"));
        selectTargetLang->setDescription(tr("팝업 번역의 목표가 되는 언어를 선택합니다."));

        DropdownMenu* selectCombo = selectTargetLang->getContent<DropdownMenu>();

        std::vector<LangType> langList = Langs::GetLanguageList();
        for (LangType lang : langList)
        {
            selectCombo->addItem(Langs::GetLocaleName(lang), static_cast<int>(lang));
        }

        connect(selectCombo, &QComboBox::currentIndexChanged, this, [this, selectCombo](const int inIdx)
        {
            const int payload = selectCombo->itemData(inIdx).toInt();

            finConfig.setPopupTargetLang(static_cast<LangType>(payload));
        });

        const LangType curTargetLang = finConfig.getPopupTargetLang();
        const int curLangIdx         = selectCombo->findData(static_cast<int>(curTargetLang));
        selectCombo->setCurrentIndex(curLangIdx);

        shapeBehaviorVLay->addWidget(selectTargetLang, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 창 위치 크기 기억
    {
        SettingCard* rememberWindow = new SettingCard(new SwitchButton(finConfig.getIsRememberWindowGeometry()), shapeBehaviorGroup);
        rememberWindow->setHeader(tr("창 위치, 크기 기억"));
        rememberWindow->setDescription(tr("다시 시작할 때, 이전 창의 위치와 크기로 복원합니다."));
        SwitchButton* remWindowSwitch = rememberWindow->getContent<SwitchButton>();
        connect(remWindowSwitch, &QCheckBox::checkStateChanged, this, [](Qt::CheckState inState)
        {
            finConfig.setIsRememberWindowGeometry(inState == Qt::CheckState::Checked);
        });
        shapeBehaviorVLay->addWidget(rememberWindow, 0, Qt::AlignmentFlag::AlignTop);
    }


    // 테마 적용 버튼
    {
        SettingCard* themeCard = new SettingCard(new QPushButton(tr("적용")), shapeBehaviorGroup);
        themeCard->setHeader(tr("테마 적용"));
        themeCard->setDescription(tr("테마를 적용합니다."));
        QPushButton* themeButton = themeCard->getContent<QPushButton>();
        themeButton->setFocusPolicy(Qt::TabFocus);
        themeButton->setCheckable(false);
        connect(themeButton, &QPushButton::clicked, this, []()
        {
            StyleManger::applyTheme();
        });
        shapeBehaviorVLay->addWidget(themeCard, 0, Qt::AlignmentFlag::AlignTop);
    }
}

GeneralOptionWidget::~GeneralOptionWidget()
{
    delete ui;
}


void GeneralOptionWidget::apply()
{
    IOptionWidget::apply();
}

void GeneralOptionWidget::cancel()
{
    IOptionWidget::cancel();
}

void GeneralOptionWidget::finish()
{
    IOptionWidget::finish();
}


// ~======================
// GeneralOption 
GeneralOption::GeneralOption()
{
    setDisplayName(tr("일반"));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new GeneralOptionWidget(); });
    setPriority(OptionPriority::GeneralOption);
}

GeneralOption::~GeneralOption()
{
}

const GeneralOption engineOption;
