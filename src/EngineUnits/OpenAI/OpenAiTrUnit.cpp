// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "OpenAiTrUnit.h"

#include "SolConstants.h"
#include "SolTranslatorCore.h"
#include "SolTypes.h"
#include "Managers/ConfigManager.h"
#include "Utils/ExJson.h"
#include "Utils/SolLog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

OpenAiTrUnit::OpenAiTrUnit(TranslateManager* parent)
    : TranslateUnit(parent)
{}

void OpenAiTrUnit::chatTranslate(const bool inIsStreaming)
{
    QNetworkRequest request(Sol::URLs::OPEN_AI);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + solConfig.apiKey(EngineType::OpenAI)).toUtf8());

    QJsonObject chatBodyJson;

    chatBodyJson["model"] = solConfig.openAIModel();
    if (inIsStreaming)
    {
        chatBodyJson["stream"] = inIsStreaming;
    }
    chatBodyJson["temperature"] = solConfig.openAI_Temperature();


    QJsonArray messages;

    QJsonObject developerMessage;
    developerMessage["role"] = "developer";
    developerMessage["content"] = QString(Sol::Prompt::OPEN_AI).arg(Langs::getEnglishName(_trReqData.sourceLang), Langs::getEnglishName(_trReqData.targetLang));
    messages.append(developerMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = _trReqData.sourceText;
    messages.append(userMessage);

    chatBodyJson["messages"] = messages;


    const QJsonDocument doc(chatBodyJson);
    const QByteArray data = doc.toJson();

    post(request, data, inIsStreaming);
}

void OpenAiTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void OpenAiTrUnit::onReadyRead()
{
    const QString content = chunkToContent();
    if (content.isEmpty() == false)
    {
        appendTranslatedText(content);
    }
}

void OpenAiTrUnit::replyTranslateFinished()
{
    if (_isStream == false)
    {
        const ExJson rootJson{_reply->readAll()};
        if (const ExJson resJson = rootJson.value("choices")[0].value("message").value("content"))
        {
            _targetText += resJson.toString();
        }
        else
        {
            solDebug << resJson.error();
        }
    }

    finishTranslateRequest(_targetText);
}

QString OpenAiTrUnit::chunkToContent()
{
    _buffer += _reply->readAll();
    QString contentStr;

    while (_buffer.isEmpty() == false)
    {
        const int pos = _buffer.indexOf("\n\n");
        if (pos < 0)
        {
            break;
        }

        QByteArray event = _buffer.left(pos);
        _buffer.remove(0, pos + 2);

        if (event.startsWith("data: ") == false)
        {
            solDebug << "not detected \'data\':" << event;
            continue;
        }

        QByteArray json = event.sliced(6);
        if (json == "[DONE]")
        {
            break;
        }

        const ExJson rootJson{json};

        // content
        const ExJson resJson = rootJson.value("choices")[0].value("delta").value("content");
        if (resJson)
        {
            contentStr += resJson.toString();
            continue;
        }


        const ExJson finishJson = rootJson.value("choices")[0].value("finish_reason");
        if (finishJson)
        {
            if (finishJson.toString() != "stop")
            {
                solDebug << "\'finish_reason\' is not \'stop\':" << finishJson.toString();
            }
            continue;
        }

        const ExJson errorJson = rootJson.value("error");
        if (errorJson)
        {
            const QJsonObject errorObj = errorJson.toObject();
            solDebug << "response error.";
            solDebug << "errorMsg:" << errorObj.value("message");
            solDebug << "errorType:" << errorObj.value("type");
            continue;
        }

        solDebug << resJson.error();
        solDebug << finishJson.error();
        solDebug << errorJson.error();
        solDebug << "last event:'" << event;
    }

    return contentStr;
}
