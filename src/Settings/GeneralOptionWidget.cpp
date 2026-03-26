// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GeneralOptionWidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SwitchButton.h"
#include "Widgets/SolMainWidget.h"


#include "Managers/StyleManger.h"

#include "SubWidgets/DropdownMenu.h"

#include "Utils/Tr.h"

using Sol::i18n;

GeneralOptionWidget::GeneralOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
{
    setObjectName("GeneralOptionWidget");

    auto [shapeBehaviorGroup, shapeBehaviorVLay] = addNewOptionGroupBox(i18n(Tr::Appearance_Behavior));

    // 시작시 실행
    {
        SettingCard* startRunCard = new SettingCard(new SwitchButton(solConfig.startRun()), shapeBehaviorGroup);
        startRunCard->setHeader(i18n(Tr::Run_On_Start));
        startRunCard->setDescription(i18n(Tr::Run_On_Start_Desc));
        SwitchButton* startRunSwitch = startRunCard->getContent<SwitchButton>();
        connect(startRunSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setStartRun(inState == Qt::CheckState::Checked);
        });
        shapeBehaviorVLay->addWidget(startRunCard, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 창 위치 크기 기억
    {
        SettingCard* rememberWindow = new SettingCard(new SwitchButton(solConfig.isRememberWindowGeometry()), shapeBehaviorGroup);
        rememberWindow->setHeader(i18n(Tr::Remember_Geometry));
        rememberWindow->setDescription(i18n(Tr::Remember_Geometry_Desc));
        const SwitchButton* remWindowSwitch = rememberWindow->getContent<SwitchButton>();
        connect(remWindowSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setIsRememberWindowGeometry(inState == Qt::CheckState::Checked);
        });
        shapeBehaviorVLay->addWidget(rememberWindow, 0, Qt::AlignmentFlag::AlignTop);
    }


    // 테마 적용 버튼
    {
        SettingCard* themeCard = new SettingCard(new QPushButton(i18n(Tr::Apply)), shapeBehaviorGroup);
        themeCard->setHeader(i18n(Tr::Apply_Theme));
        themeCard->setDescription(i18n(Tr::Apply_Theme_Desc));
        QPushButton* themeButton = themeCard->getContent<QPushButton>();
        themeButton->setFocusPolicy(Qt::TabFocus);
        themeButton->setCheckable(false);
        connect(themeButton, &QPushButton::clicked, this, []()
        {
            StyleManger::applyTheme();
        });
        shapeBehaviorVLay->addWidget(themeCard, 0, Qt::AlignmentFlag::AlignTop);
    }

    auto [popupTrGroup, popupTrVLay] = addNewOptionGroupBox(i18n(Tr::Popup_Translation));

    // 팝업번역 도착언어 선택
    {
        SettingCard* selectTargetLang = new SettingCard(new DropdownMenu, popupTrGroup);
        selectTargetLang->setHeader(i18n(Tr::Target_Language));
        selectTargetLang->setDescription(i18n(Tr::Popup_Target_Language_Desc));

        DropdownMenu* selectCombo = selectTargetLang->getContent<DropdownMenu>();

        std::vector<LangType> langList = Langs::getLanguageList();
        for (LangType lang : langList)
        {
            selectCombo->addItem(Langs::getLocaleName(lang), static_cast<int>(lang));
        }

        connect(selectCombo, &QComboBox::currentIndexChanged, this, [this, selectCombo](const int inIdx)
        {
            const int payload = selectCombo->itemData(inIdx).toInt();

            solConfig.setPopupTargetLang(static_cast<LangType>(payload));
        });

        const LangType curTargetLang = solConfig.popupTargetLang();
        const int curLangIdx         = selectCombo->findData(static_cast<int>(curTargetLang));
        selectCombo->setCurrentIndex(curLangIdx);

        popupTrVLay->addWidget(selectTargetLang, 0, Qt::AlignmentFlag::AlignTop);
    }

    // 팝업 임시창 선택
    {
        SettingCard* popupTempCard = new SettingCard(new SwitchButton(solConfig.isPopupTrWindowTemp()), popupTrGroup);
        popupTempCard->setHeader(i18n(Tr::Popup_Temp_Window));
        popupTempCard->setDescription(i18n(Tr::Popup_Temp_Window_Desc));
        SwitchButton* popupTempSwitch = popupTempCard->getContent<SwitchButton>();
        connect(popupTempSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setIsPopupTrWindowTemp(inState == Qt::CheckState::Checked);
        });
        popupTrVLay->addWidget(popupTempCard, 0, Qt::AlignmentFlag::AlignTop);
    }


    initializeAfterCtor();
}

GeneralOptionWidget::~GeneralOptionWidget()
{}



// ~======================
// GeneralOption
GeneralOption::GeneralOption()
{
    setDisplayName(i18n(Tr::General));
    setIconPath("");
    setOptionWidgetCtor([]() { return new GeneralOptionWidget(); });
    setPriority(OptionPriority::GeneralOption);
}

GeneralOption::~GeneralOption()
{}

namespace
{
const GeneralOption generalOption;
} // anonymous namespace