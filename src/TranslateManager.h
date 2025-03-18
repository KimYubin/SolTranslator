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
    struct TextCache
    {
        LangType targetLang;
        QString translateText;
    };

    hash_queue<QString, TextCache> cachingTranslateText;
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

