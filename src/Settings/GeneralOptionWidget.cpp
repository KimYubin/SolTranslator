// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GeneralOptionWidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "FinTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SwitchButton.h"
#include "Widgets/FinTranslatorMainWidget.h"


#include "Managers/StyleManger.h"

#include "SubWidgets/DropdownMenu.h"


GeneralOptionWidget::GeneralOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
{
    setObjectName("GeneralOptionWidget");

    auto [shapeBehaviorGroup, shapeBehaviorVLay] = addNewOptionGroupBox(tr("모양 및 동작"));

    // 시작시 실행
    {
        SettingCard* startRunCard = new SettingCard(new SwitchButton(finConfig.getStartRun()), shapeBehaviorGroup);
        startRunCard->setHeader(tr("시작 시 실행"));
        startRunCard->setDescription(tr("시스템 시작 시 Fin번역기가 자동으로 실행됩니다."));
        SwitchButton* startRunSwitch = startRunCard->getContent<SwitchButton>();
        connect(startRunSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            finConfig.setStartRun(inState == Qt::CheckState::Checked);
        });
        shapeBehaviorVLay->addWidget(startRunCard, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 창 위치 크기 기억
    {
        SettingCard* rememberWindow = new SettingCard(new SwitchButton(finConfig.getIsRememberWindowGeometry()), shapeBehaviorGroup);
        rememberWindow->setHeader(tr("창 위치, 크기 기억"));
        rememberWindow->setDescription(tr("다시 시작할 때, 이전 창의 위치와 크기로 복원합니다."));
        SwitchButton* remWindowSwitch = rememberWindow->getContent<SwitchButton>();
        connect(remWindowSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
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

    auto [popupTrGroup, popupTrVLay] = addNewOptionGroupBox(tr("팝업 번역"));

    // 팝업번역 도착언어 선택
    {
        SettingCard* selectTargetLang = new SettingCard(new DropdownMenu, popupTrGroup);
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

        popupTrVLay->addWidget(selectTargetLang, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 팝업 임시창 선택
    {
        SettingCard* popupTempCard = new SettingCard(new SwitchButton(finConfig.getIsPopupTrWindowTemp()), popupTrGroup);
        popupTempCard->setHeader(tr("팝업 번역창을 임시창으로 열기"));
        popupTempCard->setDescription(tr("팝업 번역창이 임시창으로 생성됩니다. 번역 중 다른 곳을 클릭하면 번역창이 닫힙니다."));
        SwitchButton* popupTempSwitch = popupTempCard->getContent<SwitchButton>();
        connect(popupTempSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            finConfig.setIsPopupTrWindowTemp(inState == Qt::CheckState::Checked);
        });
        popupTrVLay->addWidget(popupTempCard, 0, Qt::AlignmentFlag::AlignTop);
    }


    initializeAfterCtor();
}

GeneralOptionWidget::~GeneralOptionWidget()
{
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
