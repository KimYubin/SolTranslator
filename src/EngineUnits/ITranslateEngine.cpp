// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateEngine.h"

#include "Managers/EngineManager.h"
#include "Utils/EnumUtils.hpp"

#include <utility>


ITranslateEngine::ITranslateEngine(EngineId inEngine)
    : _engineId(std::move(inEngine))
    , _priority(std::numeric_limits<int>::max())
{
    EngineManager::registerEngine(this);
}

ITranslateEngine::~ITranslateEngine()
{
    EngineManager::unregisterEngine(this);
}

void ITranslateEngine::postInitialize()
{
    setIcon(QIcon{_iconPath});
}

Expected<const OptionData*> ITranslateEngine::getOptionData(const OptionKey& inKey) const
{
    const auto findIt = _optionDatas.find(inKey);
    if (findIt == _optionDatas.end())
    {
        return makeUnexpected(QString{"Not found Engine Option. Engine: %1, Key: %2"}.arg(_engineId.toString(), inKey.toString()));
    }

    return &(findIt->second);
}

std::vector<const OptionData*> ITranslateEngine::sortedOptionDataList() const
{
    std::vector<const OptionData*> resVec;
    resVec.reserve(_optionDatas.size());

    std::ranges::copy(_optionDatas | std::views::transform([](const auto& inData) { return &inData.second; }), std::back_inserter(resVec));
    std::ranges::sort(resVec, {}, &OptionData::insertionOrder);

    return resVec;
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
    setIcon(QIcon{_iconPath});
}

void ITranslateEngine::setIcon(const QIcon& inIcon)
{
    _icon = inIcon;
}

void ITranslateEngine::setPriority(const int inPriority)
{
    _priority = inPriority;
}

void ITranslateEngine::setTrUnitCreator(TrUnitCreator&& inCreator)
{
    _trUnitCreator = std::move(inCreator);
}

void ITranslateEngine::appendOptionDataList(std::vector<OptionData> inOptionDatas)
{
    _optionDatas.insert_range(inOptionDatas | std::views::transform([this](OptionData& inOpt) mutable
    {
        inOpt.insertionOrder = optionOrder();
        return std::pair{inOpt.key, std::move(inOpt)};
    }));
}

void ITranslateEngine::setOptionData(OptionData inOptionData)
{
    inOptionData.insertionOrder = optionOrder();

    _optionDatas[inOptionData.key] = std::move(inOptionData);
}
