// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionWidget.h"

#include "SettingWidgetFactory.h"
#include "SolTranslatorCore.h"
#include "EngineUnits/IAiEngine.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/OptionGroupBox.h"
#include "SubWidgets/OptionLineEdit.h"
#include "SubWidgets/SettingCard.h"
#include "Types/EngineId.h"
#include "Types/SolExpected.hpp"
#include "Utils/SolI18n.h"
#include "Utils/SolDebug.h"

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QString>
#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>


namespace Sol
{
EngineOptionWidget::EngineOptionWidget(QWidget* inParent)
    : IOptionWidget(inParent)
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
    std::vector<const OptionSpec*> optionList = inEngine->sortedOptionSpecList();
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

    for (const OptionSpec* optSpec : optionList)
    {
        switch (optSpec->getOptionSpecType())
        {
        case OptionSpec::Type::None:
            break;
        case OptionSpec::Type::Int:
            break;
        case OptionSpec::Type::Double:
            break;
        case OptionSpec::Type::Bool:
            break;
        case OptionSpec::Type::SpinDataInt:
            break;
        case OptionSpec::Type::SpinDataDouble:
        {
            doubleSpinCard(optGroup, engineId, *optSpec);
            break;
        }
        case OptionSpec::Type::StringSaver:
        {
            stringSaverCard(optGroup, engineId, *optSpec);
            break;
        }
        case OptionSpec::Type::Combo:
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
Expected<std::tuple<T, Callback<void(const T&)>>>
    EngineOptionWidget::makeSetOption(const EngineId& inEngineId
                                    , const OptionKey& inKey)
{
    Expected<QVariant> optExp = solConfig.engineOption(inEngineId, inKey);
    if (!optExp)
    {
        return makeUnexpected(optExp.error());
    }

    const T curValue  = optExp.value().value<T>();
    auto saveFunction = [inEngineId, inKey, this](const T& inVal)
    {
        const Expected<void> setExp = solConfig.setEngineOption(inEngineId, inKey, inVal);
        if (!setExp)
        {
            showErrorMessage(setExp.error());
        }
    };

    return std::tuple{curValue, saveFunction};
}

void EngineOptionWidget::stringSaverCard(OptionGroupBox* inOptGroup
                                       , const EngineId& inEngineId
                                       , const OptionSpec& inOptSpec)
{
    auto makeSetOptExp = makeSetOption<QString>(inEngineId, inOptSpec.key);
    if (!makeSetOptExp)
    {
        showErrorMessage(makeSetOptExp.error());
        return;
    }

    auto& [curValue, saveFunction] = makeSetOptExp.value();

    const Expected<SettingCard*> cardExp = CardFactory::createStringSaver(inOptGroup, inOptSpec, curValue, std::move(saveFunction));

    if (!cardExp)
    {
        showErrorMessage(cardExp.error());
        return;
    }

    SettingCard* card = cardExp.value();
    inOptGroup->addChild(card);


    // Asynchronous processing for Secret key.
    if (!inOptSpec.isSecretMode)
    {
        return;
    }

    OptionLineEdit* optLineEdit = card->getContent<OptionLineEdit>();
    const QString optDefaultStr = inOptSpec.getDefaultValue().toString();

    solConfig.loadSecretKey(inEngineId, TrEngineOptionKey::ApiKey, [this, optLinePtr = QPointer{optLineEdit}, optDefaultStr](const QString& inSecret)
    {
        if (optLinePtr)
        {
            optLinePtr->setDefaultTextAndText(optDefaultStr, inSecret);
        }
    });
}

void EngineOptionWidget::doubleSpinCard(OptionGroupBox* inOptGroup
                                      , const EngineId& inEngineId
                                      , const OptionSpec& inOptSpec)
{
    auto makeSetOptExp = makeSetOption<double>(inEngineId, inOptSpec.key);
    if (!makeSetOptExp)
    {
        showErrorMessage(makeSetOptExp.error());
        return;
    }
    auto& [curValue, saveFunction] = makeSetOptExp.value();

    Expected<SettingCard*> cardExp = CardFactory::createDoubleSpin(inOptGroup, inOptSpec, curValue, std::move(saveFunction));
    if (!cardExp)
    {
        showErrorMessage(cardExp.error());
        return;
    }

    SettingCard* card = cardExp.value();
    inOptGroup->addChild(card);
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
} // namespace Sol
