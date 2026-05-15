// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineManager.h"

#include "EngineUnits/IAiEngine.h"
#include "EngineUnits/ITranslateEngine.h"
#include "Types/EngineId.h"
#include "Utils/SolLog.h"


namespace
{
using TrEngineMap = std::unordered_map<EngineId, QPointer<ITranslateEngine>, EngineId_hasher>;

TrEngineMap& translateEngines()
{
    static TrEngineMap staticTrEngines;

    return staticTrEngines;
}
} // anonymous namespace


// ~====================
// EngineManager
EngineManager::EngineManager(SolTranslatorCore* parent)
    : AbstractManager(parent)
{
    TrEngineMap& trEngineMap = translateEngines();
    for (QPointer<ITranslateEngine>& trEngine : trEngineMap | std::views::values)
    {
        if (trEngine)
        {
            trEngine->postInitialize();
        }
    }
}

EngineManager::~EngineManager()
{}

void EngineManager::postInitialize()
{
    AbstractManager::postInitialize();
}

void EngineManager::registerEngine(ITranslateEngine* inEngine)
{
    Q_ASSERT_X(translateEngines().contains(inEngine->getEngineId()) == false, "ITranslateEngine", "Attempted to assign a duplicate EngineId. The EngineId must be unique.");

    translateEngines()[inEngine->getEngineId()] = QPointer{inEngine};
}

void EngineManager::unregisterEngine(ITranslateEngine* inEngine)
{
    translateEngines().erase(inEngine->getEngineId());
}

QPointer<ITranslateEngine> EngineManager::getEngine(const EngineId& inEngineId)
{
    Q_ASSERT_X(translateEngines().contains(inEngineId), "EngineManager::getEngine", ("EngineId is not resgitered." + inEngineId.toString()).toUtf8());

    return translateEngines()[inEngineId];
}

std::vector<QPointer<ITranslateEngine>> EngineManager::sortedTranslateEngineList()
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
        return std::tuple<int, const QString&>(inVal->getPriority(), inVal->getDisplayName());
    });

    return resVec;
}

Expected<TranslateUnit*> EngineManager::newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager)
{
    TrEngineMap& trEngineMap = translateEngines();

    const auto findIt = trEngineMap.find(inEngine);
    if (findIt == trEngineMap.end())
    {
        return makeUnexpected("TranslateManager::newTranslateUnit: not found trUnitCreator. A inEngine is " + inEngine.toString());
    }

    TranslateUnit* trUnit = findIt->second->newTrUnit(inTrManager);

    return trUnit;
}
