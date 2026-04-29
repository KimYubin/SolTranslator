// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "FinPoint/FinPointTrUnit.h"
#include "Utils/EnumUtils.hpp"


namespace
{
using TrEngineMap = std::unordered_map<EngineId, QPointer<ITranslateEngine>, EngineId_hasher>;

TrEngineMap& translateEngines()
{
    static TrEngineMap staticTrEngines;

    return staticTrEngines;
}
} // anonymous namespace

ITranslateEngine::ITranslateEngine(const EngineId& inEngine)
    : _engineId(inEngine)
    , _priority(std::numeric_limits<int>::max())
{
    Q_ASSERT_X(translateEngines().contains(_engineId) == false, "ITranslateEngine", "Attempted to assign a duplicate EngineId. The EngineId must be unique.");

    translateEngines()[_engineId] = QPointer{this};
}

ITranslateEngine::~ITranslateEngine()
{
    translateEngines().erase(_engineId);
}

std::vector<QPointer<ITranslateEngine>> ITranslateEngine::sortedTranslateEngineList()
{
    TrEngineMap& trEngineMap = translateEngines();

    // cleanup nullptr
    std::erase_if(trEngineMap, [](const auto& inVal)
    {
        return inVal.second.isNull();
    });

    std::vector<QPointer<ITranslateEngine>> resVec;
    resVec.reserve(trEngineMap.size());

    std::ranges::copy(trEngineMap | std::views::values, std::back_inserter(resVec));
    std::ranges::sort(resVec, {}, [](const QPointer<ITranslateEngine>& inVal)
    {
        return std::tie(inVal->_priority, inVal->_displayName);
    });

    return resVec;
}

std::expected<TranslateUnit*, QString> ITranslateEngine::newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager)
{
    TrEngineMap& trEngineMap = translateEngines();

    const auto findIt = trEngineMap.find(inEngine);
    if (findIt == trEngineMap.end())
    {
        return std::unexpected{"TranslateManager::newTranslateUnit: not found trUnitCreator. A inEngine is " + inEngine.toString()};
    }

    TranslateUnit* trUnit = findIt->second->_trUnitCreator(inTrManager);

    return trUnit;
}

void ITranslateEngine::setDisplayName(const QString& inDisplayName)
{
    _displayName = inDisplayName;
}

void ITranslateEngine::setDefaultUrl(const QString& inDefaultUrl)
{
    _defaultUrl = inDefaultUrl;
}

void ITranslateEngine::setDefaultPrompt(const QString& inDefaultPrompt)
{
    _defaultPrompt = inDefaultPrompt;
}

void ITranslateEngine::setIconPath(const QString& inIconPath)
{
    _iconPath = inIconPath;
}

void ITranslateEngine::setPriority(const int inPriority)
{
    _priority = inPriority;
}

void ITranslateEngine::setTrUnitCreator(TrUnitCreator&& inCreator)
{
    _trUnitCreator = std::move(inCreator);
}
