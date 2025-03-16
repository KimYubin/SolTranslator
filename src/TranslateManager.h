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
                     , const QString& sourceLang
                     , const QString& targetLang);

    void translateSimple(const QString& text
                       , const QString& sourceLang
                       , const QString& targetLang);

private slots:

public:
    void setCacheText(const QString& originText
                    , const QString& translateText
                    , const QString& targetLang);

    std::tuple<bool, QString> findCachingText(const QString& originText, const QString& targetLang);

private:
    int maxCacheLength = 50;
    struct TextCache
    {
        QString targetLang;
        QString translateText;
    };

    hash_queue<QString, TextCache> cachingTranslateText;
};



template <typename Func>
void TranslateManager::translateText(const FunctorContextType<Func>* inTextEditableObj
                                   , Func&& slotfunctor
                                   , const QString& text
                                   , const QString& sourceLang
                                   , const QString& targetLang)
{
    TranslateUnit* tranUnit = new TranslateUnit(this);
    tranUnit->translateText(inTextEditableObj, std::forward<Func>(slotfunctor), text, sourceLang, targetLang);
}

#endif //TRANSLATEMANAGER_H

