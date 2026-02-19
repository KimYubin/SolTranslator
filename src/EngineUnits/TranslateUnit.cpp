// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateUnit.h"

#include <QNetworkReply>

#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"

#include "Widgets/ITranslateWidget.h"

TranslateUnit::TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                           , TranslateManager* parent)
    : QObject(parent)
    , _trReqData(inTranslateRequestInfo)
{
    Q_ASSERT(_trReqData.trDisplayWidget);
    _trReqData.trDisplayWidget->setTrUnit(this);
}

void TranslateUnit::executeTextTranslation()
{
    if (_trReqData.originText.isEmpty())
    {
        solDebug << "translate request text is empty";

        completeTranslatedText(_trReqData.originText);
        return;
    }
    HistoryManager* historyManager = solCore->historyManager();
    if (_trReqData.bIgnoreCache == false && historyManager != nullptr)
    {
        auto [bIsFind, findCache] = historyManager->lookupHistory(_trReqData.engineType
                                                                , _trReqData.originText
                                                                , _trReqData.sourceLang
                                                                , _trReqData.targetLang);
        if (bIsFind)
        {
            completeTranslatedText(findCache);
            return;
        }
    }

    // to subclass
    requestTranslate();
}

void TranslateUnit::get(const QNetworkRequest& request)
{
    _reply = solCore->translateManager()->getNetworkAccessManager()->get(request);
    postProcess();
}

void TranslateUnit::post(const QNetworkRequest& request, const QByteArray& data, const bool bIsStreaming)
{
    _reply = solCore->translateManager()->getNetworkAccessManager()->post(request, data);

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
            replyFailed();
        }
        _reply->deleteLater();
    }
    deleteLater();
}

void TranslateUnit::disconnectTranslateDisplay()
{
    _trReqData.trDisplayWidget->setTrUnit(nullptr);
    _trReqData.trDisplayWidget            = nullptr;
    _trReqData.streamContext              = nullptr;
    _trReqData.callbackTranslateStreaming = nullptr;
    _trReqData.completeContext            = nullptr;
    _trReqData.callbackTranslateComplete  = nullptr;
}

void TranslateUnit::detachDisplayWidget()
{
    disconnectTranslateDisplay();

    if (_reply.isNull())
    {
        deleteLater();
    }
}

void TranslateUnit::abortTranslateRequest()
{
    if (_reply)
    {
        _reply->abort();
    }
    solDebug << "abort translate request";

    disconnectTranslateDisplay();

    deleteLater();
}

void TranslateUnit::replyFailed()
{
    solDebug << "Error: " << _reply->errorString();
    solDebug << "EngineType:" << sol::enumToQStr(_trReqData.engineType);
    solDebug << "Source Text:" << _trReqData.originText.left(50);

    // 사용자가 history에서 재번역 시도를 할 수 있습니다.
    finishTranslateRequest(_reply->errorString());
}

void TranslateUnit::addTranslatedText(const QString& inTranslatedText)
{
    _translatedText.append(inTranslatedText);

    if (_trReqData.streamContext && _trReqData.callbackTranslateStreaming)
    {
        (*_trReqData.callbackTranslateStreaming)(_translatedText);
    }
}

void TranslateUnit::updateHistory(const QString& inTranslatedText)
{
    if (inTranslatedText.isEmpty())
    {
        return;
    }

    if (HistoryManager* historyManager = solCore->historyManager())
    {
        historyManager->addHistory(_trReqData.engineType
                                 , _trReqData.sourceLang
                                 , _trReqData.targetLang
                                 , _trReqData.originText
                                 , inTranslatedText
                                 , _trReqData.textFormat);
    }
}

void TranslateUnit::completeTranslatedText(const QString& inTranslatedText)
{
    // 빈 문자열도 적용합니다.
    if (_trReqData.completeContext && _trReqData.callbackTranslateComplete)
    {
        _trReqData.callbackTranslateComplete(inTranslatedText);
    }

    deleteLater();
}

void TranslateUnit::finishTranslateRequest(const QString& inTranslatedText)
{
    updateHistory(inTranslatedText);
    completeTranslatedText(inTranslatedText);
}
