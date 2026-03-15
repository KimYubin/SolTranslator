// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "OpenAiTrUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "SolConstants.h"
#include "SolLog.h"
#include "SolTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "Utils/SolJson.h"

OpenAiTrUnit::OpenAiTrUnit(TranslateManager* parent)
    : TranslateUnit(parent)
{}

void OpenAiTrUnit::chatTranslate(const bool inIsStreaming)
{
    QNetworkRequest request(sol::URLs::OPEN_AI);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + solConfig.getAPIKey(EngineType::OpenAI)).toStdString().c_str());

    QJsonObject chatBodyJson;

    chatBodyJson["model"] = solConfig.getOpenAIModel();
    if (inIsStreaming)
    {
        chatBodyJson["stream"] = inIsStreaming; // streaming
    }
    chatBodyJson["temperature"] = solConfig.getOpenAI_Temperature();


    QJsonArray messages;

    QJsonObject developerMessage;
    developerMessage["role"] = "developer";
    developerMessage["content"] = QString(sol::Prompt::OPEN_AI).arg(Langs::GetEnglishName(_trReqData.sourceLang), Langs::GetEnglishName(_trReqData.targetLang));
    messages.append(developerMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = _trReqData.originText;
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
        addTranslatedText(content);
    }
}

void OpenAiTrUnit::replyTranslateFinished()
{
    const QByteArray responseData = _reply->readAll();

    // chatComplete no streaming.
    if (responseData.isEmpty() == false)
    {
        const SolJson rootJson{responseData};
        if (const SolJson::Expected& resExp = rootJson.value("choices")[0].value("message").value("content").expected())
        {
            _translatedText += resExp.value().toString();
        }
        else
        {
            solDebug << resExp.error();
        }
    }

    finishTranslateRequest(_translatedText);
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

        const SolJson rootJson{json};

        // content
        const SolJson::Expected& resExp = rootJson.value("choices")[0].value("delta").value("content").expected();
        if (resExp)
        {
            contentStr += resExp.value().toString();
            continue;
        }


        const SolJson::Expected& finishExp = rootJson.value("choices")[0].value("finish_reason").expected();
        if (finishExp)
        {
            if (finishExp.value() != "stop")
            {
                solDebug << "\'finish_reason\' is not \'stop\':" << finishExp.value();
            }
            continue;
        }

        const SolJson::Expected& errorExp = rootJson.value("error").expected();
        if (errorExp)
        {
            const QJsonObject errorObj = errorExp.value().toObject();
            solDebug << "response error.";
            solDebug << "errorMsg:" << errorObj.value("message");
            solDebug << "errorType:" << errorObj.value("type");
            continue;
        }

        solDebug << resExp.error();
        solDebug << finishExp.error();
        solDebug << errorExp.error();
        solDebug << "last event:'" << event;
    }

    return contentStr;
}
