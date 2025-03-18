//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H

#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <unordered_set>

#include "FinHashQueue.h"
#include "TlUnitFactory.h"
#include "TranslateUnit.h"

class QTextEdit;
class TranslateUnit;

template <typename Func>
using FunctorContextType = typename QtPrivate::ContextTypeForFunctor<Func>::ContextType;


class TranslateManager : public QObject
{
    Q_OBJECT

public:
    TranslateManager(QObject* parent = nullptr);

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

    void SetEngineType(EngineType inEngine) { currentEngine = inEngine; }
    EngineType GetCurrentEngineType() const { return currentEngine; };

private:
    EngineType currentEngine;

    // ~===========
    // cache
    int maxCacheLength = 50;

    struct TextCacheKey
    {
        QString originText;
        EngineType engineType;
        LangType targetLang;
    };

    struct cache_ky_hasher
    {
        size_t operator()(const TextCacheKey& inKy) const
        {
            return std::hash<::QString>()(inKy.originText
                + QString::fromStdString(std::to_string(EnumToInt(inKy.engineType)) + std::to_string(EnumToInt(inKy.targetLang))));
        }
    };

    struct cache_ky_eq
    {
        bool operator()(const TextCacheKey& ACacheKy, const TextCacheKey& BCacheKy) const
        {
            return (ACacheKy.engineType == BCacheKy.engineType)
                    && (ACacheKy.targetLang == BCacheKy.targetLang)
                    && (ACacheKy.originText == BCacheKy.originText);
        }
    };

    /**
     * 캐시된 번역문을 관리합니다.
     * 원문, 엔진, 목표언어를 key로 사용합니다.
     * 최대치를 갱신하면, 캐시된 번역문은 선입선출로 삭제됩니다.  
     */
    hash_queue<TextCacheKey, QString, cache_ky_hasher, cache_ky_eq> cachingTranslateText;
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

