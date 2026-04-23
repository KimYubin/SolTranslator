// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "FinPoint/FinPointTrUnit.h"
#include "Utils/EnumUtils.hpp"


namespace
{
TrEngineMap& translateEngineMap()
{
    static TrEngineMap staticOptionPages;

    return staticOptionPages;
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

const TrEngineMap& ITranslateEngine::allTrUnitCreators()
{
    return translateEngineMap();
}

std::vector<ITranslateEngine*> ITranslateEngine::sortedTranslateEngineList()
{
    std::vector<ITranslateEngine*> vecEngines;
    const auto& translateEngines = translateEngineMap();
    for (const auto& trEngine : translateEngines | std::views::values)
    {
        if (trEngine)
        {
            vecEngines.push_back(trEngine);
        }
    }

    std::ranges::sort(vecEngines, {}, [](ITranslateEngine* a)
    {
        return a->getPriority();
    });

    return vecEngines;
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
