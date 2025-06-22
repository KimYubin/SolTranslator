//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateUnit.h"

#include <QNetworkReply>

#include "../FinTypes.h"
#include "../ConfigManager.h"
#include "../TranslateManager.h"

TranslateUnit::TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                           , TranslateManager* parent)
    : QNetworkAccessManager(parent)
    , _trReqData(inTranslateRequestInfo)
{
}

void TranslateUnit::executeTextTranslation()
{
    if (_trReqData.originText.isEmpty())
    {
        qDebug()<<"translate request text is empty";
    }
    if (_trReqData.callbackTranslateStreaming.has_value())
    {
        connect(this, &TranslateUnit::addStreamTranslatedText, _trReqData.streamContext, std::move((*_trReqData.callbackTranslateStreaming)));
    }

    connect(this, &TranslateUnit::onCompletedTranslate, _trReqData.completeContext, std::move(_trReqData.callbackTranslateComplete));

    connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
    
    executeTextTranslation_Impl();
}

void TranslateUnit::executeTextTranslation_Impl()
{
    if (_trReqData.originText.isEmpty())
    {
        completeTranslatedText(_trReqData.originText);
        return;
    }

    if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(_trReqData.originText, _trReqData.targetLang);
        if (bIsFind)
        {
            // 캐싱되어있다면 업데이트 합니다.
            // 내부에서 캐시의 순서를 최신으로 변경합니다.
            completeTranslatedText(findCache);
            return;
        }
    }

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

void TranslateUnit::abortTranslate()
{
    if (_reply.isNull() == false)
    {
        qDebug() << "abort translate request";
        _reply->abort();
    }
}

void TranslateUnit::addTranslatedText(const QString& inTranslatedText)
{
    _translatedText.append(inTranslatedText);
    emit addStreamTranslatedText(_translatedText);
}

void TranslateUnit::completeTranslatedText(const QString& inTranslatedText)
{
    if (inTranslatedText.isEmpty() == false)
    {
        if (TranslateManager* translate_manager = qobject_cast<TranslateManager*>(parent()))
        {
            translate_manager->setCacheText(_trReqData.originText, inTranslatedText, _trReqData.targetLang);
        }
    }

    // 빈 문자열도 적용합니다.
    emit onCompletedTranslate(inTranslatedText);
    deleteLater();
}
