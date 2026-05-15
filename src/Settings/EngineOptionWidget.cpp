// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "SettingWidgetFactory.h"
#include "SolTranslatorCore.h"
#include "EngineUnits/IAiEngine.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/OptionGroupBox.h"
#include "SubWidgets/SettingCard.h"
#include "Types/EngineId.h"
#include "Types/SolExpected.hpp"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

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

    const QString& displayName = inEngine->getDisplayName();
    const EngineId& engineId   = inEngine->getEngineId();

    QWidget* layoutWidget   = new QWidget(_tabWidget);
    QGridLayout* gridLayout = new QGridLayout(layoutWidget);

    _tabWidget->addTab(layoutWidget, inEngine->getIcon(), displayName);

    OptionGroupBox* optGroup
        = OptionWidgetFactory::createOptionGroupBox(displayName + " " + i18n(Tr::Options)
                                                  , gridLayout
                                                  , gridLayout->rowCount()
                                                  , 0);

    for (const OptionData* optData : optionList)
    {
        switch (optData->getOptionDataType())
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
            doubleSpinCard(optGroup, engineId, *optData);
            break;
        }
        case OptionData::Type::StringSaver:
        {
            stringSaverCard(optGroup, engineId, *optData);
            break;
        }
        case OptionData::Type::Combo:
            break;
        default: ;
        }
    }
}

void EngineOptionWidget::showErrorMessage(const Error& inError)
{
    solDebug << inError;
}

template <typename T>
Expected<std::tuple<T, MoveFunc<void(const T&)>>>
    EngineOptionWidget::makeSetAttribute(const EngineId& inEngineId
                                       , const OptionKey& inKey)
{
    Expected<QVariant> attrExp = solConfig.engineAttribute(inEngineId, inKey);
    if (!attrExp)
    {
        return makeUnexpected(attrExp.error());
    }

    const T curValue  = attrExp.value().value<T>();
    auto saveFunction = [inEngineId, inKey, this](const T& inVal)
    {
        const Expected<void> setExp = solConfig.setEngineAttribute(inEngineId, inKey, inVal);
        if (!setExp)
        {
            showErrorMessage(setExp.error());
        }
    };

    return std::tuple{curValue, saveFunction};
}

void EngineOptionWidget::stringSaverCard(OptionGroupBox* inOptGroup
                                       , const EngineId& inEngineId
                                       , const OptionData& inOptData)
{
    auto makeSetAttrExp = makeSetAttribute<QString>(inEngineId, inOptData.key);
    if (!makeSetAttrExp)
    {
        showErrorMessage(makeSetAttrExp.error());
        return;
    }

    auto& [curValue, saveFunction] = makeSetAttrExp.value();

    Expected<SettingCard*> card = CardFactory::createStringSaver(inOptGroup, inOptData, curValue, std::move(saveFunction));
    if (card)
    {
        inOptGroup->addChild(card.value());
    }
}

void EngineOptionWidget::doubleSpinCard(OptionGroupBox* inOptGroup
                                      , const EngineId& inEngineId
                                      , const OptionData& inOptData)
{
    auto makeSetAttrExp = makeSetAttribute<double>(inEngineId, inOptData.key);
    if (!makeSetAttrExp)
    {
        showErrorMessage(makeSetAttrExp.error());
        return;
    }
    auto& [curValue, saveFunction] = makeSetAttrExp.value();

    Expected<SettingCard*> card = CardFactory::createDoubleSpin(inOptGroup, inOptData, curValue, std::move(saveFunction));
    if (card)
    {
        inOptGroup->addChild(card.value());
    }
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
