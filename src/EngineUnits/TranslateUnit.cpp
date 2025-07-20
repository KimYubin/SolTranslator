// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateUnit.h"

#include <QNetworkReply>

#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

TranslateUnit::TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                           , TranslateManager* parent)
    : QObject(parent)
    , _translateManager(parent)
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

    // connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
    
    // executeTextTranslation_Impl();
    
    if (_trReqData.originText.isEmpty())
    {
        completeTranslatedText(_trReqData.originText);
        return;
    }

    if (TranslateManager* translate_manager = qobject_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(_trReqData.engineType
                                                                     , _trReqData.originText
                                                                     , _trReqData.sourceLang
                                                                     , _trReqData.targetLang);
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

void TranslateUnit::get(const QNetworkRequest& request)
{
    _reply = _translateManager->getNetworkAccessManager()->get(request);

    connect(_reply.data(), &QNetworkReply::finished, this, &TranslateUnit::onReplyFinished);
}

void TranslateUnit::post(const QNetworkRequest& request, const QByteArray& data, const bool bIsStreaming)
{
    _reply = _translateManager->getNetworkAccessManager()->post(request, data);

    if (bIsStreaming)
    {
        connect(_reply.data(), &QIODevice::readyRead, this, [this]() { onReadyRead(_reply); });
    }

    connect(_reply.data(), &QNetworkReply::finished, this, &TranslateUnit::onReplyFinished);
}

void TranslateUnit::executeTextTranslation_Impl()
{
}

void TranslateUnit::onReadyRead(QNetworkReply* reply)
{
    qDebug()<<"onReadyRead";
}

void TranslateUnit::onReplyFinished(/*QNetworkReply* reply*/)
{
    if (_reply->error() == QNetworkReply::NoError)
    {
        // to subclass
        replyTranslateFinished(_reply);
    }
    else
    {
        qDebug() << "Error: " << _reply->errorString();
    }
    _reply->deleteLater();

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
            translate_manager->setCacheText(_trReqData.engineType
                                          , _trReqData.originText
                                          , inTranslatedText
                                          , _trReqData.sourceLang
                                          , _trReqData.targetLang);
        }
    }

    // 빈 문자열도 적용합니다.
    emit onCompletedTranslate(inTranslatedText);
    deleteLater();
}
