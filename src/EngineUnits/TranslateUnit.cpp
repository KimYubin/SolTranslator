// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateUnit.h"

#include <QNetworkReply>

#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "Widgets/ITranslateWidget.h"

TranslateUnit::TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                           , TranslateManager* parent)
    : QObject(parent)
    , _trReqData(inTranslateRequestInfo)
{
    Q_ASSERT(_trReqData.trTargetWidget);
    _trReqData.trTargetWidget->setTrUnit(this);
}

void TranslateUnit::executeTextTranslation()
{
    if (_trReqData.originText.isEmpty())
    {
        qDebug()<<"translate request text is empty";
        
        completeTranslatedText(_trReqData.originText);
        return;
    }

    if (TranslateManager* translate_manager = finCore->translateManager())
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
    _reply = finCore->translateManager()->getNetworkAccessManager()->get(request);
    postProcess();
}

void TranslateUnit::post(const QNetworkRequest& request, const QByteArray& data, const bool bIsStreaming)
{
    _reply = finCore->translateManager()->getNetworkAccessManager()->post(request, data);

    if (bIsStreaming)
    {
        connect(_reply, &QIODevice::readyRead, this, &TranslateUnit::onReadyRead);
    }
    postProcess();
}

void TranslateUnit::postProcess()
{
    connect(_reply, &QNetworkReply::finished, this, &TranslateUnit::onReplyFinished);
    connect(_reply, &QObject::destroyed, this, &QObject::deleteLater); // reply 오류에 대비
}

void TranslateUnit::onReplyFinished()
{
    if (_reply.isNull() == false)
    {
        if (_reply->error() == QNetworkReply::NoError)
        {
            // to subclass
            replyTranslateFinished();
        }
        else
        {
            qDebug() << "Error: " << _reply->errorString();
        }
        _reply->deleteLater();
    }
    deleteLater();
}

void TranslateUnit::abortTranslate()
{
    if (_reply.isNull() == false)
    {
        qDebug() << "abort translate request";
        _reply->abort();

        _trReqData.streamContext              = nullptr;
        _trReqData.callbackTranslateStreaming = nullptr;
        _trReqData.completeContext            = nullptr;
        _trReqData.callbackTranslateComplete  = nullptr;
    }
}

void TranslateUnit::addTranslatedText(const QString& inTranslatedText)
{
    _translatedText.append(inTranslatedText);

    if (_trReqData.streamContext && _trReqData.callbackTranslateStreaming)
    {
        (*_trReqData.callbackTranslateStreaming)(_translatedText);
    }
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
    if (_trReqData.completeContext && _trReqData.callbackTranslateComplete)
    {
        _trReqData.callbackTranslateComplete(inTranslatedText);
    }

    deleteLater();
}
