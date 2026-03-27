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

TranslateUnit::TranslateUnit(TranslateManager* parent)
    : QObject(parent)
    , _trReqData()
{}

std::expected<void, QString> TranslateUnit::executeTextTranslation(TranslateRequestInfo&& inTranslateRequestInfo)
{
    _trReqData = std::move(inTranslateRequestInfo);

    if (_trReqData.trDisplayWidget)
    {
        _trReqData.trDisplayWidget->setTrUnit(this);
    }

    if (_trReqData.originText.isEmpty())
    {
        solDebug << "translate request text is empty";

        completeTranslatedText(_trReqData.originText);
        return{};
    }

    if (_trReqData.isIgnoreCache)
    {
        requestTranslate();
        return{};
    }

    HistoryManager* historyManager = solCore->historyManager();
    if (historyManager == nullptr)
    {
        return std::unexpected{"historyManager does not exist"};
    }

    historyManager->asyncLookupHistory(
        _trReqData.engineType
      , _trReqData.originText
      , _trReqData.sourceLang
      , _trReqData.targetLang
      , this
      , [inThis = QPointer{this}, this](const std::tuple<bool, QString>& inRes)
        {
            if (inThis.isNull())
            {
                solDebug << "The trUnit was destroyed before the database lookup was completed.";
                return;
            }
            auto& [isFind, findCache] = inRes;
            if (isFind)
            {
                completeTranslatedText(findCache);
            }
            else
            {
                requestTranslate();
            }
        });
    return {};
}

void TranslateUnit::get(const QNetworkRequest& inRequest)
{
    _reply = solCore->translateManager()->get(inRequest);
    postProcess();
}

void TranslateUnit::post(const QNetworkRequest& inRequest, const QByteArray& inPayload, const bool inIsStreaming)
{
    _isStream = inIsStreaming;

    _reply = solCore->translateManager()->post(inRequest, inPayload);

    if (_isStream)
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
    if (_trReqData.trDisplayWidget)
    {
        _trReqData.trDisplayWidget->setTrUnit(nullptr);
        _trReqData.trDisplayWidget = nullptr;
    }

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
    solDebug << "EngineType:" << Sol::enumToQStr(_trReqData.engineType);
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
        historyManager->asyncAddHistory(_trReqData.engineType
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
    _translatedText = inTranslatedText;
    updateHistory(inTranslatedText);
    completeTranslatedText(inTranslatedText);
}
