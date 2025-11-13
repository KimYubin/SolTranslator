// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include "FinTranslatorCore.h"


HistoryManager::HistoryManager(FinTranslatorCore* parent) : AbstractManager(parent)
{
    
}


void HistoryManager::setTranslationHistory(const EngineType inEngineType
                                         , const QString& inOriginText
                                         , const QString& inTranslateText
                                         , const LangType inSourceLang
                                         , const LangType inTargetLang)
{
    // 이미 캐시되어 있다면, 순서 최신화
    _cachingTranslateText.push(TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang}, inTranslateText);
    if (_cachingTranslateText.size() > _maxCacheLength)
    {
        _cachingTranslateText.pop();
    }

    // 캐시 저장
    finCore->asyncSaveCache();
}

std::tuple<bool, QString> HistoryManager::findHistory(const EngineType inEngineType
                                                    , const QString& inOriginText
                                                    , const LangType inSourceLang
                                                    , const LangType inTargetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const TextCacheKey findCacheKey = TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang};
    if (const QString* text_cache = _cachingTranslateText.find(findCacheKey))
    {
        res = {true, *text_cache};
    }

    return res;
}

void HistoryManager::updateNewCacheQueue(cache_queue&& newCache)
{
    _cachingTranslateText = std::move(newCache);
}

const cache_queue& HistoryManager::getCacheQueue() const
{
    return _cachingTranslateText;
}
