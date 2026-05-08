// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "SettingWidgetFactory.h"
#include "SolTranslatorCore.h"
#include "EngineUnits/IAiEngine.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/SettingCard.h"
#include "Types/EngineId.h"
#include "Types/SolExpected.hpp"
#include "Utils/SolI18n.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QString>
#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>

using Sol::i18n;

EngineOptionWidget::EngineOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
{
    setObjectName("EngineOptionWidget");

    _tabWidget = new QTabWidget(this);
    _mainLayout->addWidget(_tabWidget);

    // 엔진별 옵션 위젯 생성
    std::vector<QPointer<ITranslateEngine>> engines = EngineManager::sortedTranslateEngineList();
    for (const QPointer<ITranslateEngine>& engine : engines)
    {
        addEngineSettings(engine.data());
    }

    initializeAfterCtor();
}

EngineOptionWidget::~EngineOptionWidget()
{}

void EngineOptionWidget::addEngineSettings(const ITranslateEngine* inEngine)
{
    std::vector<const OptionData*> optionList = inEngine->sortedOptionDataList();
    if (optionList.empty())
    {
        return;
    }

    QWidget* layoutWidget   = new QWidget(_tabWidget);
    QGridLayout* gridLayout = new QGridLayout(layoutWidget);

    _tabWidget->addTab(layoutWidget, inEngine->getDisplayName());

    const EngineId& engineId = inEngine->getEngineId();
    auto [optGroup, optVLay] = OptionWidgetFactory::createOptionGroupBox(inEngine->getDisplayName() + " " + i18n(Tr::Options)
                                                                       , gridLayout
                                                                       , gridLayout->rowCount()
                                                                       , 0);

    for (const OptionData* optData : optionList)
    {
        Expected<SettingCard*> settingCard;
        switch (optData->getOptionType())
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
            settingCard = doubleSpinCard(optGroup, engineId, *optData);
            break;
        }
        case OptionData::Type::StringSaver:
        {
            settingCard = stringSaverCard(optGroup, engineId, *optData);
        }
        break;
        case OptionData::Type::Combo:
            break;
        default: ;
        }

        if (settingCard)
        {
            optVLay->addWidget(settingCard.value(), 0, Qt::AlignmentFlag::AlignTop);
        }
    }
}

namespace
{
template <typename T>
std::tuple<T, std::move_only_function<void(const T&)>> makeSetAttribute(const EngineId& inEngineId
                                                                      , const OptionKey& inKey)
{
    const T curValue  = solConfig.engineAttribute(inEngineId, inKey).value<T>();
    auto saveFunction = [inEngineId, inKey](const T& inVal)
    {
        solConfig.setEngineAttribute(inEngineId, inKey, inVal);
    };

    return {curValue, saveFunction};
}
} // anonymous namespace

Expected<SettingCard*> EngineOptionWidget::stringSaverCard(QWidget* inParent
                                                         , const EngineId& inEngineId
                                                         , const OptionData& inOptData)
{
    auto [curValue, saveFunction] = makeSetAttribute<QString>(inEngineId, inOptData.key);

    return CardFactory::createStringSaver(inParent, inOptData, curValue, std::move(saveFunction));
}

Expected<SettingCard*> EngineOptionWidget::doubleSpinCard(QWidget* inParent
                                                        , const EngineId& inEngineId
                                                        , const OptionData& inOptData)
{
    auto [curValue, saveFunction] = makeSetAttribute<double>(inEngineId, inOptData.key);

    return CardFactory::createDoubleSpin(inParent, inOptData, curValue, std::move(saveFunction));
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
