// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "FinPoint/FinPointTrUnit.h"
#include "Utils/EnumUtils.hpp"


namespace
{
using TrEngineMap = std::unordered_map<EngineId, QPointer<ITranslateEngine>, EngineId_hasher>;

TrEngineMap& translateEngineMap()
{
    static TrEngineMap staticTrEngines;

    return staticTrEngines;
}
} // anonymous namespace

ITranslateEngine::ITranslateEngine(const EngineId& inEngine)
    :_engineId(inEngine)
{
    _priority = std::numeric_limits<int>::max();
    translateEngineMap()[_engineId] = this;
}

ITranslateEngine::~ITranslateEngine()
{
    translateEngineMap().erase(_engineId);
}

std::vector<QPointer<ITranslateEngine>> ITranslateEngine::sortedTranslateEngineList()
{
    TrEngineMap& trEngines = translateEngineMap();

    // cleanup nullptr
    std::erase_if(trEngines, [](const auto& inVal)
    {
        return inVal.second.isNull();
    });

    std::vector<QPointer<ITranslateEngine>> resVec;
    resVec.reserve(trEngines.size());

    std::ranges::copy(trEngines | std::views::values, std::back_inserter(resVec));
    std::ranges::sort(resVec, {}, [](const QPointer<ITranslateEngine>& inVal)
    {
        return std::tie(inVal->_priority, inVal->_displayName);
    });

    return resVec;
}

std::expected<TranslateUnit*, QString> ITranslateEngine::newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager)
{
    const auto findIt = translateEngineMap().find(inEngine);
    if (findIt == translateEngineMap().end())
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
