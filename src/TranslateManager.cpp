//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateManager.h"


#include "ConfigManager.h"
#include "FinHashQueue.h"
#include "FinTypes.h"
#include "simpletranslatewidget.h"
#include "TranslateUnit.h"

TranslateManager::TranslateManager(QObject* parent): QObject(parent)
{
    SetEngineType(EngineType::OpenAI);
}

void TranslateManager::translateSimple(const QString& text
                                     , const LangType sourceLang
                                     , const LangType targetLang)
{
    SimpleTranslateWidget* simple = new SimpleTranslateWidget();
    translateText(simple, &SimpleTranslateWidget::showTranslationPopup, text, sourceLang, targetLang);
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

