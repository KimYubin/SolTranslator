// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H


#include "AbstractManager.h"
#include "FinHashQueue.h"


class QNetworkAccessManager;
class QTextEdit;
class TranslateUnit;
class FinTranslatorCore;
class QMimeData;


class TranslateManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit TranslateManager(FinTranslatorCore* parent);

    TranslateUnit* executeNewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo);
    
    QPointer<TranslateUnit> translateText(const TranslateRequestInfo& inTranslateRequestInfo);
    
    void translateSimple(const QMimeData* inMimeData
                       , const LangType inSourceLang
                       , const LangType inTargetLang);

private slots:

public:
    void setCacheText(const EngineType inEngineType
                    , const QString& inOriginText
                    , const QString& inTranslateText
                    , const LangType inSourceLang
                    , const LangType inTargetLang);

    std::tuple<bool, QString> findCachingText(const EngineType inEngineType
                                            , const QString& inOriginText
                                            , const LangType inSourceLang
                                            , const LangType inTargetLang);


    void updateNewCacheQueue(cache_queue&& newCache);
    const cache_queue& getCacheQueue() const;

    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; };

private:
    QNetworkAccessManager* _networkAccessManager;
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



#endif //TRANSLATEMANAGER_H

