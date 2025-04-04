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

#include <QMimeData>

#include "FinTranslatorCore.h"


TranslateManager::TranslateManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    SetEngineType(EngineType::OpenAI);
}

QPointer<TranslateUnit> TranslateManager::translateText(const TranslateRequestInfo& inTranslateRequestInfo)
{
    TranslateUnit* tranUnit = TlUnitFactory::get().NewTranslateUnit(inTranslateRequestInfo, this);
    tranUnit->executeTextTranslation();

    return QPointer<TranslateUnit>{tranUnit};
}

void TranslateManager::translateSimple(const QMimeData* inMimeData
                                     , const LangType inSourceLang
                                     , const LangType inTargetLang)
{
    SimpleTranslatePopup* simple = new SimpleTranslatePopup(getFinCore());

    // 마크다운 변환
    QString originText;
    TextStyle textStyle;

    if (inMimeData->hasText() == false)
    {
        return;
    }

    if (inMimeData->hasHtml())
    {
        QTextDocument txtDoc;
        // list 무시하는 문법 제거.
        txtDoc.setHtml(inMimeData->html().replace(QRegularExpression(R"(list-style: none)"), ""));
        originText = txtDoc.toMarkdown();
        textStyle  = TextStyle::MarkDown;
    }
    else
    {
        originText = inMimeData->text();
        textStyle  = TextStyle::PlainText;
    }


    QPointer<TranslateUnit> transUnit = translateText(TranslateRequestInfo{
        originText
      , textStyle
      , inSourceLang
      , inTargetLang
      , simple
      , [=](const QString& inStr) { simple->completeTransText(inStr, textStyle); }
      , simple
      , [=](const QString& inStr) { simple->streamTransText(inStr, textStyle); }
    });

    connect(simple, &SimpleTranslatePopup::abortTranslateReq, transUnit, [=]()
    {
        if (transUnit.isNull())
        {
            return;
        }
        transUnit->abortTranslate();
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

    // 캐시 저장
    getFinCore()->asyncSaveCache();
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

