// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "Managers/EngineManager.h"
#include "Utils/EnumUtils.hpp"


ITranslateEngine::ITranslateEngine(const EngineId& inEngine)
    : _engineId(inEngine)
    , _priority(std::numeric_limits<int>::max())
{
    EngineManager::registerEngine(this);
}

ITranslateEngine::~ITranslateEngine()
{
    EngineManager::unregisterEngine(this);
}

Expected<const OptionData*>ITranslateEngine::getOptionData(const OptionKey& inKey)
{
    auto findIt = _optionDatas.find(inKey);
    if (findIt != _optionDatas.end())
    {
        return &findIt->second;
    }

    return std::unexpected{"not found OptionKey: " + inKey.toString()};
}

TranslateUnit* ITranslateEngine::newTrUnit(TranslateManager* inTrManager)
{
    return _trUnitCreator(inTrManager);
}

void ITranslateEngine::setDisplayName(const QString& inDisplayName)
{
    _displayName = inDisplayName;
}

void ITranslateEngine::setDefaultUrl(const QString& inDefaultUrl)
{
    _defaultUrl = inDefaultUrl;
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

void ITranslateEngine::appendOptionDataList(const std::vector<OptionData>& inOptionDatas)
{
    _optionDatas.insert_range(inOptionDatas | std::views::transform([](const OptionData& inOpt)
    {
        return std::pair{inOpt.key, inOpt};
    }));
}

void ITranslateEngine::setOptionData(const OptionData& inOptionData)
{
    _optionDatas[inOptionData.key] = inOptionData;
}
