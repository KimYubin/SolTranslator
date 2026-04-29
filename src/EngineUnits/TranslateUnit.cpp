// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TranslateUnit.h"

#include "ITranslateEngine.h"
#include "SolTranslatorCore.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "Types/SolGuard.h"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolLog.h"
#include "Widgets/ITranslateWidget.h"

#include <QNetworkReply>
#include <QNetworkRequest>

TranslateUnit::TranslateUnit(TranslateManager* parent, ITranslateEngine* inEngine)
    : QObject(parent)
    , _translateManager(parent)
    , _translateEngine(inEngine)
    , _trReqData()
{
    Q_ASSERT_X(_translateManager, "TranslateUnit::TranslateUnit", "_translateManager is invalid.");
    Q_ASSERT_X(_translateEngine, "TranslateUnit::TranslateUnit", "_translateEngine is invalid.");
}

TranslateUnit::~TranslateUnit()
{
    if (_reply)
    {
        _reply->deleteLater();
    }
}

void TranslateUnit::setTranslateRequest(TranslateRequest&& inTrRequest)
{
    _trReqData = std::move(inTrRequest);

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
    if (_reply.isNull())
    {
        replyFailed(replyErrorString());
        return;
    }

    connect(_reply, &QNetworkReply::finished, this, &TranslateUnit::onReplyFinished);
    connect(_reply, &QNetworkReply::errorOccurred, this, &TranslateUnit::onReplyErrorOccurred);
    connect(_reply, &QObject::destroyed, this, &TranslateUnit::onReplyDestroyed); // Prepare for reply errors.
}

void TranslateUnit::cleanUpReply()
{
    if (_reply)
    {
        _reply->deleteLater();
    }
    _reply = nullptr;
}

// ~==========
// slots
void TranslateUnit::onReplyFinished()
{
    if (_isReplyFinished)
    {
        return;
    }
    _isReplyFinished = true;

    if (_reply.isNull() || _reply->error() != QNetworkReply::NoError)
    {
        replyFailed(replyErrorString());
        return;
    }

    finishRequest(replyTranslateFinished());
}

void TranslateUnit::onReplyErrorOccurred(/*const QNetworkReply::NetworkError inNetworkError*/)
{
    if (_isReplyFinished)
    {
        return;
    }
    _isReplyFinished = true;

    replyFailed(replyErrorString());
}

void TranslateUnit::onReplyDestroyed()
{
    if (_isReplyFinished)
    {
        return;
    }
    _isReplyFinished = true;

    replyFailed("network reply destroyed unexpectedly");
}


//~========

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

void TranslateUnit::abortRequest()
{
    if (_reply)
    {
        _reply->abort();
    }
    solDebug << "abort translate request";

    disconnectTranslateDisplay();

    deleteLater();
}

QString TranslateUnit::replyErrorString() const
{
    if (_reply)
    {
        return _reply->errorString();
    }
    return "_reply is empty!";
}

void TranslateUnit::replyFailed(const QString& inReason)
{
    solDebug << "Error: " << inReason;
    solDebug << "EngineId:" << _trReqData.engineId.toString();
    solDebug << "Source Text:" << _trReqData.sourceText.left(50);

    cleanUpReply();

    // User can attempt to re-translate from the history.
    const QString msg = _targetText + "\nrequest error: " + inReason;
    finishRequest(msg);

    // Called in finishRequest().
    // deleteLater();
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

void TranslateUnit::finishRequest(const QString& inTargetText)
{
    _targetText = inTargetText;
    addHistory(_targetText);
    completeTranslatedText(_targetText);
}
