// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINHASHQUEUE_H
#define FINHASHQUEUE_H
#include <unordered_map>

#include <QString>
#include <QHashFunctions>

#include "FinTypes.h"


/**
 * 원소가 추가된 순서를 유지하는 해시 맵입니다. 
 * value의 선입선출을 유지하고, key를 통해 삭제할 수 있습니다.
 * 탐색 및 삽입, 삭제 모두 상수시간 복잡도를 가집니다.
 */
template <typename _Kty, typename _Valty, typename _Hasher= std::hash<_Kty>, typename _Keyeq = std::equal_to<_Kty>>
class hash_queue
{
    using pair_list = std::list<std::pair<_Kty, _Valty>>;

    pair_list keyValQueue; // key, value queue

    std::unordered_map<_Kty, typename pair_list::iterator, _Hasher, _Keyeq> keyListHash; // key, list_iterator 매핑 테이블

public:
    hash_queue() = default;

    // ~===================
    // copy & move
    hash_queue(const hash_queue& inOther)
        : keyValQueue(inOther.keyValQueue)
    {
        keyListHash.reserve(inOther.keyListHash.size());
        for (auto it = keyValQueue.begin(); it != keyValQueue.end(); ++it)
        {
            keyListHash[it->first] = it;
        }
    }

    hash_queue(hash_queue&& inOther) noexcept
        : keyValQueue(std::move(inOther.keyValQueue))
        , keyListHash(std::move(inOther.keyListHash))
    {}

    hash_queue& operator=(const hash_queue& inOther)
    {
        if (this == &inOther)
            return *this;

        keyValQueue = inOther.keyValQueue;
        keyListHash.reserve(inOther.keyListHash.size());
        for (auto it = keyValQueue.begin(); it != keyValQueue.end(); ++it)
        {
            keyListHash[it->first] = it;
        }
        return *this;
    }

    hash_queue& operator=(hash_queue&& inOther) noexcept
    {
        if (this == &inOther)
            return *this;

        keyValQueue = std::move(inOther.keyValQueue);
        keyListHash = std::move(inOther.keyListHash);
        return *this;
    }


    // ~=====================
    // queue interface

    void push(const _Kty& key, const _Valty& value)
    {
        auto findHashIt = keyListHash.find(key);
        if (findHashIt != keyListHash.end())
        {
            if (findHashIt->second->second != value)
            {
                findHashIt->second->second = value;
            }

            keyValQueue.splice(keyValQueue.end(), keyValQueue, findHashIt->second);
            return;
        }

        // 리스트에 삽입 및 해시 테이블에 위치 저장
        keyListHash[key] = keyValQueue.emplace(keyValQueue.end(), key, value);
    }

    _Valty top()
    {
        if (keyValQueue.empty())
        {
            return {};
        }

        return keyValQueue.front().second;
    }

    void pop()
    {
        if (keyValQueue.empty())
        {
            return;
        }

        keyListHash.erase(keyValQueue.front().first);
        keyValQueue.pop_front();
    }

    bool erase(const _Kty& key)
    {
        const auto findIt = keyListHash.find(key);
        if (findIt == keyListHash.end())
        {
            return false;
        }

        keyValQueue.erase(findIt->second);
        keyListHash.erase(findIt);
        return true;
    }

    _Valty* find(const _Kty& key)
    {
        const auto findIt = keyListHash.find(key);
        if (findIt == keyListHash.end())
        {
            return nullptr;
        }

        return &(findIt->second->second);
    }

    /** 키를 조회합니다. key가 있다면 순서를 갱신합니다. */
    _Valty* look_up(const _Kty& key)
    {
        const auto findIt = keyListHash.find(key);
        if (findIt == keyListHash.end())
        {
            return nullptr;
        }

        keyValQueue.splice(keyValQueue.end(), keyValQueue, findIt->second);
        return &(findIt->second->second);
    }

    bool empty()
    {
        return keyValQueue.empty();
    }

    size_t size()
    {
        return keyValQueue.size();
    }

    //~ =====================
    // 반복자
    using iterator       = typename pair_list::iterator;
    using const_iterator = typename pair_list::const_iterator;

    // begin(), end() 제공
    iterator begin() noexcept
    {
        return keyValQueue.begin();
    }

    iterator end() noexcept
    {
        return keyValQueue.end();
    }

    const_iterator begin() const noexcept
    {
        return keyValQueue.begin();
    }

    const_iterator end() const noexcept
    {
        return keyValQueue.end();
    }

    const_iterator cbegin() const noexcept
    {
        return keyValQueue.cbegin();
    }

    const_iterator cend() const noexcept
    {
        return keyValQueue.cend();
    }
};


struct TextCacheKey
{
    TextCacheKey() = default;

    TextCacheKey(const EngineType inEngineType
               , const QString& inOriginText
               , const LangType inSourceLang
               , const LangType inTargetLang)
        : engineType(inEngineType)
        , originText(inOriginText)
        , sourceLang(inSourceLang)
        , targetLang(inTargetLang) {}

    EngineType engineType;
    QString originText;
    LangType sourceLang;
    LangType targetLang;
};

struct cache_ky_hasher
{
    size_t operator()(const TextCacheKey& inKy) const
    {
        return std::hash<::QString>()(
            inKy.originText
            + QChar(EnumToInt(inKy.engineType))
            + QChar(EnumToInt(inKy.sourceLang))
            + QChar(EnumToInt(inKy.targetLang)));
    }
};

struct cache_ky_eq
{
    bool operator()(const TextCacheKey& ACacheKy, const TextCacheKey& BCacheKy) const
    {
        return (ACacheKy.engineType == BCacheKy.engineType)
                && (ACacheKy.sourceLang == BCacheKy.sourceLang)
                && (ACacheKy.targetLang == BCacheKy.targetLang)
                && (ACacheKy.originText == BCacheKy.originText);
    }
};

/**
 * 캐시된 번역문을 관리합니다.
 * 원문, 엔진, 목표언어를 key로 사용합니다.
 * 최대치를 갱신하면, 캐시된 번역문은 선입선출로 삭제됩니다.  
 */
using cache_queue = hash_queue<TextCacheKey, QString, cache_ky_hasher, cache_ky_eq>;


#endif //FINHASHQUEUE_H
