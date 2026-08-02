// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "GeneralOptionWidget.h"

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/StyleManger.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/OptionGroupBox.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SwitchButton.h"
#include "Types/SolTypes.h"
#include "Utils/SolI18n.h"
#include "Widgets/SolMainWidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

using Sol::i18n;

GeneralOptionWidget::GeneralOptionWidget(QWidget* inParent)
    : IOptionWidget(inParent)
{
    setObjectName("GeneralOptionWidget");

    OptionGroupBox* shapeBehaviorGroup = addNewOptionGroupBox(i18n(Tr::Appearance_Behavior));

    // 시작시 실행
    {
        SettingCard* startRunCard = new SettingCard(new SwitchButton(solConfig.startRun()), shapeBehaviorGroup);
        startRunCard->setHeader(i18n(Tr::Run_On_Start));
        startRunCard->setDescription(i18n(Tr::Run_On_Start_Desc));

        const SwitchButton* startRunSwitch = startRunCard->getContent<SwitchButton>();
        connect(startRunSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setStartRun(inState == Qt::CheckState::Checked);
        });

        shapeBehaviorGroup->addChild(startRunCard);
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

        shapeBehaviorGroup->addChild(rememberWindow);
    }

    // 툴팁에서 단축키 보이기
    {
        SettingCard* showToolTip = new SettingCard(new SwitchButton(solConfig.isShowToolTipShortcut()), shapeBehaviorGroup);
        showToolTip->setHeader(i18n(Tr::Show_Shortcut_ToolTip));
        showToolTip->setDescription(i18n(Tr::Show_Shortcut_ToolTip_Desc));
        const SwitchButton* remWindowSwitch = showToolTip->getContent<SwitchButton>();
        connect(remWindowSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setIsShowToolTipShortcut(inState == Qt::CheckState::Checked);
        });

        shapeBehaviorGroup->addChild(showToolTip);
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
            StyleManger::applyTheme(/*dark*/);
        });

        shapeBehaviorGroup->addChild(themeCard);
    }


    OptionGroupBox* popupTrGroup = addNewOptionGroupBox(i18n(Tr::Popup_Translation));

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

        popupTrGroup->addChild(selectTargetLang);
    }

    // 팝업 임시창 선택
    {
        SettingCard* popupTempCard = new SettingCard(new SwitchButton(solConfig.isPopupTrWindowTemp()), popupTrGroup);
        popupTempCard->setHeader(i18n(Tr::Popup_Temp_Window));
        popupTempCard->setDescription(i18n(Tr::Popup_Temp_Window_Desc));

        const SwitchButton* popupTempSwitch = popupTempCard->getContent<SwitchButton>();
        connect(popupTempSwitch, &QCheckBox::checkStateChanged, this, [](const Qt::CheckState inState)
        {
            solConfig.setIsPopupTrWindowTemp(inState == Qt::CheckState::Checked);
        });

        popupTrGroup->addChild(popupTempCard);
    }


    initializeAfterCtor();
}

GeneralOptionWidget::~GeneralOptionWidget()
{}



// ~======================
// GeneralOption
GeneralOptionPage::GeneralOptionPage()
{
    setDisplayName(i18n(Tr::General));
    setIconPath("");
    setOptionWidgetCtor([]() { return new GeneralOptionWidget(); });
    setPriority(0);
}

GeneralOptionPage::~GeneralOptionPage()
{}

namespace
{
const GeneralOptionPage generalOptionPage;
} // anonymous namespace