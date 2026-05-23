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

Expected<const OptionSpec*> ITranslateEngine::getOptionSpec(const OptionKey& inKey) const
{
    const auto findIt = _optionSpecs.find(inKey);
    if (findIt == _optionSpecs.end())
    {
        return makeUnexpected(QString{"Not found Engine Option. Engine: %1, Key: %2"}.arg(_engineId.toString(), inKey.toString()));
    }

    return &(findIt->second);
}

std::vector<const OptionSpec*> ITranslateEngine::sortedOptionSpecList() const
{
    std::vector<const OptionSpec*> resVec;
    resVec.reserve(_optionSpecs.size());

    std::ranges::copy(_optionSpecs | std::views::transform([](const auto& inOpt) { return &inOpt.second; }), std::back_inserter(resVec));
    std::ranges::sort(resVec, {}, &OptionSpec::insertionOrder);

    return resVec;
}

std::vector<QString> ITranslateEngine::secretEngineOptionKeys() const
{
    std::vector<QString> resVec;
    resVec.reserve(_optionSpecs.size());

    std::ranges::copy(
        _optionSpecs
        | std::views::filter([](const auto& inOpt) { return inOpt.second.isSecretMode; })
        | std::views::transform([this](const auto& inOpt) { return Sol::engineOptionKey(_engineId, inOpt.second.key); })
      , std::back_inserter(resVec)
    );

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

void ITranslateEngine::appendOptionSpecList(std::vector<OptionSpec> inOptionSpecs)
{
    _optionSpecs.insert_range(inOptionSpecs | std::views::transform([this](OptionSpec& inOpt) mutable
    {
        inOpt.insertionOrder = optionOrder();
        return std::pair{inOpt.key, std::move(inOpt)};
    }));
}

void ITranslateEngine::setOptionSpec(OptionSpec inOptionSpec)
{
    inOptionSpec.insertionOrder = optionOrder();

    _optionSpecs[inOptionSpec.key] = std::move(inOptionSpec);
}
