// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYMANAGER_H
#define FINTRANSLATOR_HISTORYMANAGER_H



#include "AbstractManager.h"
#include "FinHashQueue.h"
#include "FinTypes.h"

class FinTranslatorCore;
class QSqlError;

class HistoryManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit HistoryManager(FinTranslatorCore* parent);
    virtual ~HistoryManager() override;

private:
    QSqlError initializeDB();

public:
    void addHistory(const EngineType inEngineType
                  , const LangType inSourceLang
                  , const LangType inTargetLang
                  , const QString& inOriginText
                  , const QString& inTranslateText);

    std::tuple<bool, QString> findHistory(const EngineType inEngineType
                                        , const QString& inOriginText
                                        , const LangType inSourceLang
                                        , const LangType inTargetLang);
    /**
     * 번역 기록이 있다면, 번역문을 반환합니다.
     * 해당 번역의 최근 기록을 추가합니다.
     * 
     * @param inEngineType 
     * @param inOriginText 
     * @param inSourceLang 
     * @param inTargetLang 
     * @return 
     */
    std::tuple<bool, QString> lookupHistory(const EngineType inEngineType
                                          , const QString& inOriginText
                                          , const LangType inSourceLang
                                          , const LangType inTargetLang);

    void updateNewCacheQueue(cache_queue&& newCache);
    const cache_queue& getCacheQueue() const;

private:
    // ~===========
    // cache
    int _maxCacheLength = 100;

    /**
     * 캐시된 번역문을 관리합니다.
     * 원문, 엔진, 목표언어를 key로 사용합니다.
     * 최대치를 갱신하면, 캐시된 번역문은 선입선출로 삭제됩니다.  
     */
    cache_queue _cachingTranslateText;
};


#endif //FINTRANSLATOR_HISTORYMANAGER_H
