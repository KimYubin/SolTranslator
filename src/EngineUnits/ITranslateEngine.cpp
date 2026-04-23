// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "FinPoint/FinPointTrUnit.h"
#include "Utils/EnumUtils.hpp"


namespace
{
TrEngineMap& trUnitCreators()
{
    static TrEngineMap staticOptionPages;

    return staticOptionPages;
}
} // anonymous namespace


ITranslateEngine::ITranslateEngine(const EngineType inEngine)
    : _engineType(inEngine)
{
    trUnitCreators()[_engineType] = this;
}

ITranslateEngine::~ITranslateEngine()
{
    trUnitCreators().erase(_engineType);
}

const TrEngineMap& ITranslateEngine::allTrUnitCreators()
{
    return trUnitCreators();
}

TranslateUnit* ITranslateEngine::newTrUnit(const EngineType inEngine, TranslateManager* inTrManager)
{
    const auto findIt = trUnitCreators().find(inEngine);
    if (findIt == trUnitCreators().end())
    {
        Q_ASSERT_X(
            findIt != trUnitCreators().end()
          , "TranslateManager::newTranslateUnit"
          , ("not found trUnitCreator. A inEngine is " + Sol::enumToQStr<EngineType>(inEngine)).toUtf8()
        );
        return nullptr;
    }

    TranslateUnit* trUnit = findIt->second->_trUnitCreator(inTrManager);

    if (inEngine == EngineType::FinPointDebug)
    {
        static_cast<FinPointTrUnit*>(trUnit)->setDebugMode(true);
    }

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

void ITranslateEngine::setTrUnitCreator(TrUnitCreator&& inCreator)
{
    _trUnitCreator = std::move(inCreator);
}
