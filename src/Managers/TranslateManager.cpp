// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include <QMimeData>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QTextDocument>

#include "AsyncManager.h"
#include "ConfigManager.h"
#include "FinHashQueue.h"
#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "EngineUnits/TrUnitFactory.h"
#include "EngineUnits/TranslateUnit.h"
#include "Widgets/SimpleTranslatePopup.h"


TranslateManager::TranslateManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    
}

QPointer<TranslateUnit> TranslateManager::translateText(const TranslateRequestInfo& inTranslateRequestInfo)
{
    TranslateUnit* transUnit = TrUnitFactory::instance().NewTranslateUnit(inTranslateRequestInfo, this);
    transUnit->executeTextTranslation();

    return QPointer<TranslateUnit>{transUnit};
}

void TranslateManager::translateSimple(const QMimeData* inMimeData
                                     , const LangType inSourceLang
                                     , const LangType inTargetLang)
{
    if (inMimeData->hasText() == false)
    {
        return;
    }

    SimpleTranslatePopup* simple = new SimpleTranslatePopup();
    auto runSimpleTranslate = [=, this](const QString& inOriginText, const TextStyle inTextStyle)
    {
        QPointer<TranslateUnit> transUnit = translateText(TranslateRequestInfo{
            finConfig.getCurrentEngineType()
          , inOriginText
          , inTextStyle
          , inSourceLang
          , inTargetLang
          , simple
          , [=](const QString& inStr) { simple->completeTransText(inStr, inTextStyle); }
          , simple
          , [=](const QString& inStr) { simple->streamTransText(inStr, inTextStyle); }
        });

        connect(simple, &SimpleTranslatePopup::abortTranslateReq, transUnit, [=]()
        {
            if (transUnit.isNull())
            {
                return;
            }
            transUnit->abortTranslate();
        });
    };


    if (inMimeData->hasHtml())
    {
        AsyncManager::asyncLaunch<QString>(
            simple,
            [=, htmlStr = std::move(inMimeData->html())]() mutable
            {
                // list 무시하는 문법 제거.
                QTextDocument txtDoc;
                txtDoc.setHtml(htmlStr.replace(QRegularExpression(R"(list-style: none)"), ""));

                return txtDoc.toMarkdown();
            },
            [=](const QString& inMd)
            {
                runSimpleTranslate(inMd, TextStyle::MarkDown);
            });
    }
    else
    {
        runSimpleTranslate(inMimeData->text(), TextStyle::PlainText);
    }
}

void TranslateManager::setCacheText(const EngineType inEngineType
                                  , const QString& inOriginText
                                  , const QString& inTranslateText
                                  , const LangType inSourceLang
                                  , const LangType inTargetLang)
{
    // 이미 캐시되어 있다면, 순서 최신화
    _cachingTranslateText.push(TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang}, inTranslateText);
    if (_cachingTranslateText.size() > _maxCacheLength)
    {
        _cachingTranslateText.pop();
    }

    // 캐시 저장
    finCore->asyncSaveCache();
}

std::tuple<bool, QString> TranslateManager::findCachingText(const EngineType inEngineType
                                                          , const QString& inOriginText
                                                          , const LangType inSourceLang
                                                          , const LangType inTargetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const TextCacheKey findCacheKey = TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang};
    if (const QString* text_cache = _cachingTranslateText.find(findCacheKey))
    {
        res = {true, *text_cache};
    }

    return res;
}

void TranslateManager::updateNewCacheQueue(cache_queue&& newCache)
{
    _cachingTranslateText = std::move(newCache);
}

const cache_queue& TranslateManager::getCacheQueue() const
{
    return _cachingTranslateText;
}

