// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TranslateUnit.h"

#include "SolTranslatorCore.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolLog.h"
#include "Widgets/ITranslateWidget.h"

#include <QNetworkReply>
#include <QNetworkRequest>

TranslateUnit::TranslateUnit(TranslateManager* parent)
    : QObject(parent)
    , _translateManager(parent)
    , _trReqData()
{
    Q_ASSERT_X(_translateManager, "TranslateUnit::TranslateUnit", "TranslateManager is invalid.");
}

void TranslateUnit::setTranslateRequestInfo(TranslateRequestInfo&& inTranslateRequestInfo)
{
    _trReqData = std::move(inTranslateRequestInfo);

    if (_trReqData.trDisplayWidget)
    {
        _trReqData.trDisplayWidget->setTrUnit(this);
    }

}

void TranslateUnit::onTranslationFromCache(const QString& inTargetText)
{
    completeTranslatedText(inTargetText);
}

void TranslateUnit::get(const QNetworkRequest& inRequest)
{
    Q_ASSERT_X(_translateManager, "TranslateUnit::get", "TranslateManager is invalid.");

    _reply = _translateManager->get(inRequest);
    postProcess();
}

void TranslateUnit::post(const QNetworkRequest& inRequest, const QByteArray& inPayload, const bool inIsStreaming)
{
    Q_ASSERT_X(_translateManager, "TranslateUnit::get", "TranslateManager is invalid.");

    _reply = _translateManager->post(inRequest, inPayload);

    _isStream = inIsStreaming;
    if (_isStream)
    {
        connect(_reply, &QIODevice::readyRead, this, &TranslateUnit::onReadyRead);
    }
    postProcess();
}

void TranslateUnit::postProcess()
{
    connect(_reply, &QNetworkReply::finished, this, &TranslateUnit::onReplyFinished);
    connect(_reply, &QObject::destroyed, this, &QObject::deleteLater); // Prepare for reply errors.
}

void TranslateUnit::onReplyFinished()
{
    if (_reply.isNull() == false)
    {
        if (_reply->error() == QNetworkReply::NoError)
        {
            finishTranslateRequest(replyTranslateFinished());
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
    solDebug << "Source Text:" << _trReqData.sourceText.left(50);

    // User can attempt to re-translate from the history.
    finishTranslateRequest(_targetText + "\nrequest error: " + _reply->errorString());
}

void TranslateUnit::appendTranslatedText(const QString& inDeltaTargetText)
{
    _targetText.append(inDeltaTargetText);

    if (_trReqData.streamContext && _trReqData.callbackTranslateStreaming)
    {
        _trReqData.callbackTranslateStreaming.value()(_targetText);
    }
}

void TranslateUnit::addHistory(const QString& inTargetText)
{
    _translateManager->onAddHistoryRequested(_trReqData, inTargetText);
}

void TranslateUnit::completeTranslatedText(const QString& inTargetText)
{
    // Also Apply to empty strings.
    if (_trReqData.completeContext && _trReqData.callbackTranslateComplete)
    {
        _trReqData.callbackTranslateComplete(inTargetText);
    }

    deleteLater();
}

void TranslateUnit::finishTranslateRequest(const QString& inTargetText)
{
    _targetText = inTargetText;
    addHistory(_targetText);
    completeTranslatedText(_targetText);
}
