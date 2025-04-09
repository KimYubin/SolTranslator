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

#include <QFutureWatcher>
#include <QMimeData>
#include <QRegularExpression>
#include <QtConcurrentRun>

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
    if (inMimeData->hasText() == false)
    {
        return;
    }

    auto runSimpleTranslate = [=](const QString& inOriginText, const TextStyle inTextStyle)
    {
        SimpleTranslatePopup* simple = new SimpleTranslatePopup(getFinCore());

        QPointer<TranslateUnit> transUnit = translateText(TranslateRequestInfo{
            inOriginText
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
        // 비동기 문법 개선 및 마크다운 변환 작업
        QFuture<QString> future = QtConcurrent::run([=, htmlStr = std::move(inMimeData->html())]() mutable
        {
            // list 무시하는 문법 제거.
            QTextDocument txtDoc;
            txtDoc.setHtml(htmlStr.replace(QRegularExpression(R"(list-style: none)"), ""));

            return txtDoc.toMarkdown();
            // 메인 스레드로 복귀
            QMetaObject::invokeMethod(qApp, [=, originText = std::move(txtDoc.toMarkdown())]()
            {
                runSimpleTranslate(originText, TextStyle::MarkDown);
            }, Qt::QueuedConnection);
        });

        QFutureWatcher<QString>* dataWatcher = new QFutureWatcher<QString>(this);
        connect(dataWatcher, &QFutureWatcher<QString>::finished, this, [=]
        {
            runSimpleTranslate(dataWatcher->future().result(), TextStyle::MarkDown);
            dataWatcher->deleteLater();
        });

        dataWatcher->setFuture(future);

    }
    else
    {
        runSimpleTranslate(inMimeData->text(), TextStyle::PlainText);
    }
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

