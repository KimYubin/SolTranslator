//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateUnit.h"

#include <iostream>
#include <QNetworkReply>

#include "ConfigManager.h"
#include "FinTypes.h"
#include "TranslateManager.h"

TranslateUnit::TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                           , TranslateManager* parent)
    : QNetworkAccessManager(parent)
    , trReqData(inTranslateRequestInfo)
{
}

void TranslateUnit::executeTextTranslation()
{
    if (trReqData.callbackTranslateStreaming.has_value())
    {
        connect(this, &TranslateUnit::addStreamTranslatedText, trReqData.streamContext, std::move((*trReqData.callbackTranslateStreaming)));
    }

    connect(this, &TranslateUnit::onCompletedTranslate, trReqData.completeContext, std::move(trReqData.callbackTranslateComplete));
    executeTextTranslation_Impl();
}

void TranslateUnit::executeTextTranslation_Impl()
{
    if (trReqData.originText.isEmpty())
    {
        updateTranslatedText(trReqData.originText);
        return;
    }

    if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(trReqData.originText, trReqData.targetLang);
        if (bIsFind)
        {
            // 캐싱되어있다면 업데이트 합니다.
            // 내부에서 캐시의 순서를 최신으로 변경합니다.
            updateTranslatedText(findCache);
            return;
        }
    }

    connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
    // to subclass
    requestTranslate();

}

void TranslateUnit::onReplyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // to subclass
        replyTranslateFinished(reply);
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();

    deleteLater();
}

void TranslateUnit::updateTranslatedText(const QString& inTranslatedText)
{
    if (inTranslatedText.isEmpty() == false)
    {
        if (TranslateManager* translate_manager = qobject_cast<TranslateManager*>(parent()))
        {
            translate_manager->setCacheText(trReqData.originText, inTranslatedText, trReqData.targetLang);
        }
    }

    // 빈 문자열도 적용합니다.
    emit onCompletedTranslate(inTranslatedText);
    deleteLater();
}

void TranslateUnit::addTranslatedText(const QString& inTranslatedText)
{
    translatedText.append(inTranslatedText);
    emit addStreamTranslatedText(translatedText);
}
