//
// Created by YubinKim on 25/03/10 월.
//
#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>

#include "ConfigManager.h"
#include "FinHashQueue.h"
#include "FinTypes.h"
#include "SimpleTranslatePopup.h"
#include "TranslateUnit.h"
#include "TlUnitFactory.h"

#include "TranslateManager.h"


TranslateManager::TranslateManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    SetEngineType(EngineType::OpenAI);
}

void TranslateManager::translateText(const TranslateRequestInfo& inTranslateRequestInfo)
{
    TranslateUnit* tranUnit = TlUnitFactory::get().NewTranslateUnit(inTranslateRequestInfo, this);
    tranUnit->executeTextTranslation();
}

void TranslateManager::translateSimple(const QString& inOrignText
                                     , const LangType inSourceLang
                                     , const LangType inTargetLang)
{
    SimpleTranslatePopup* simple = new SimpleTranslatePopup(getFinCore());

    translateText(TranslateRequestInfo{
        inOrignText
      , inSourceLang
      , inTargetLang
      , simple
      , [=](const QString& inStr) { simple->showTranslationPopup(inStr); }
      , simple
      , [=](const QString& inStr) { simple->showTranslationPopup(inStr); }
    });
}

void TranslateManager::setCacheText(const QString& originText, const QString& translateText, const LangType targetLang)
{
    // 중복은 순서 최신화
    cachingTranslateText.push({originText, currentEngine, targetLang}, translateText);
    if (cachingTranslateText.size() > maxCacheLength)
    {
        cachingTranslateText.pop();
    }
}

std::tuple<bool, QString> TranslateManager::findCachingText(const QString& originText, const LangType targetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const TextCacheKey findCacheKey = TextCacheKey{originText, currentEngine, targetLang};
    if (const QString* text_cache = cachingTranslateText.find(findCacheKey))
    {
        res = {true, *text_cache};
    }

    return res;
}

void TranslateManager::updateNewCacheQueue(cache_queue&& newCache)
{
    cachingTranslateText = std::move(newCache);
}

const cache_queue& TranslateManager::getCacheQueue() const
{
    return cachingTranslateText;
}

