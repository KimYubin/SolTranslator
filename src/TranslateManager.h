//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H

#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <unordered_set>

#include "AbstractManager.h"
#include "FinHashQueue.h"
#include "TlUnitFactory.h"
#include "TranslateUnit.h"

class QTextEdit;
class TranslateUnit;
class FinTranslatorCore;

template <typename Func>
using FunctorContextType = typename QtPrivate::ContextTypeForFunctor<Func>::ContextType;


class TranslateManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit TranslateManager(FinTranslatorCore* parent);

    template <typename Func>
    void translateText(const FunctorContextType<Func>* inTextEditableObj
                     , Func&& slotfunctor
                     , const QString& text
                     , const LangType sourceLang
                     , const LangType targetLang);

    void translateSimple(const QString& text
                       , const LangType sourceLang
                       , const LangType targetLang);

private slots:

public:
    void setCacheText(const QString& originText
                    , const QString& translateText
                    , const LangType targetLang);

    std::tuple<bool, QString> findCachingText(const QString& originText, const LangType targetLang);


    void updateNewCacheQueue(cache_queue&& newCache);
    const cache_queue& getCacheQueue() const; 
    
    void SetEngineType(EngineType inEngine) { currentEngine = inEngine; }
    EngineType GetCurrentEngineType() const { return currentEngine; };

private:
    EngineType currentEngine;

    // ~===========
    // cache
    int maxCacheLength = 100;

    /**
     * 캐시된 번역문을 관리합니다.
     * 원문, 엔진, 목표언어를 key로 사용합니다.
     * 최대치를 갱신하면, 캐시된 번역문은 선입선출로 삭제됩니다.  
     */
    cache_queue cachingTranslateText;
};



template <typename Func>
void TranslateManager::translateText(const FunctorContextType<Func>* inTextEditableObj
                                   , Func&& slotfunctor
                                   , const QString& text
                                   , const LangType sourceLang
                                   , const LangType targetLang)
{
    TranslateUnit* tranUnit = TlUnitFactory::get().NewTranslateUnit(this);
    tranUnit->translateText(inTextEditableObj, std::forward<Func>(slotfunctor), text, sourceLang, targetLang);
}

#endif //TRANSLATEMANAGER_H

