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
}


void TranslateManager::translateSimple(const QString& text
                                     , const QString& sourceLang
                                     , const QString& targetLang)
{
    SimpleTranslateWidget* simple = new SimpleTranslateWidget();
    translateText(simple, &SimpleTranslateWidget::showTranslationPopup, text, sourceLang, targetLang);
}

void TranslateManager::setCacheText(const QString& originText, const QString& translateText, const QString& targetLang)
{
    if (cachingTranslateText.size() >= maxCacheLength)
    {
        cachingTranslateText.pop();
    }
    cachingTranslateText.push(originText, {targetLang, translateText});
}

std::tuple<bool, QString> TranslateManager::findCachingText(const QString& originText, const QString& targetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    if (const TextCache* text_cache = cachingTranslateText.find(originText))
    {
        TextCache newTextCache = std::move(*text_cache);
        if (newTextCache.targetLang == targetLang)
        {
            res = {true, newTextCache.translateText};
        }

        // cachingTranslateText.erase(originText);
        cachingTranslateText.push(originText, newTextCache);
    }

    return res;
}

