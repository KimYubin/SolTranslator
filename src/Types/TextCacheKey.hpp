// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_TEXTCACHEKEY_H
#define SOLTRANSLATOR_TEXTCACHEKEY_H


#include "EngineId.h"
#include "SolHashQueue.hpp"
#include "SolTypes.h"
#include "Utils/EnumUtils.hpp"

#include <QHashFunctions>
#include <QString>

struct TextCacheKey
{
    TextCacheKey() = default;

    TextCacheKey(const EngineId& inEngineId
               , const QString& inSourceText
               , const LangType inSourceLang
               , const LangType inTargetLang)
        : engineId(inEngineId)
        , sourceText(inSourceText)
        , sourceLang(inSourceLang)
        , targetLang(inTargetLang)
    {}

    EngineId engineId;
    QString sourceText;
    LangType sourceLang;
    LangType targetLang;
};

struct cache_ky_hasher
{
    size_t operator()(const TextCacheKey& inKy) const
    {
        return std::hash<::QString>()(
            inKy.sourceText
            + inKy.engineId.toString()
            + QChar(Sol::enumToInt(inKy.sourceLang))
            + QChar(Sol::enumToInt(inKy.targetLang))
        );
    }
};

struct cache_ky_eq
{
    bool operator()(const TextCacheKey& inACacheKy, const TextCacheKey& inBCacheKy) const
    {
        return (inACacheKy.engineId == inBCacheKy.engineId)
                && (inACacheKy.sourceLang == inBCacheKy.sourceLang)
                && (inACacheKy.targetLang == inBCacheKy.targetLang)
                && (inACacheKy.sourceText == inBCacheKy.sourceText);
    }
};

/**
 * 캐시된 번역문을 관리합니다.
 * 원문, 엔진, 목표언어를 key로 사용합니다.
 * 최대치를 갱신하면, 캐시된 번역문은 선입선출로 삭제됩니다.  
 */
using cache_queue = hash_queue<TextCacheKey, QString, cache_ky_hasher, cache_ky_eq>;


#endif //SOLTRANSLATOR_TEXTCACHEKEY_H
