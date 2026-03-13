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
    // chatComplete no streaming
    const QByteArray responseData    = _reply->readAll();
    const QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
    const QJsonObject jsonObject     = responseJson.object();
    const QJsonArray choices         = jsonObject["choices"].toArray();
    if (choices.isEmpty() == false)
    {
        const QString lastTranslatedText = choices.first().toObject()["message"].toObject()["content"].toString();
        _translatedText.append(lastTranslatedText);
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
            break;
        }

        QByteArray json = event.sliced(6);

        if (json == "[DONE]")
        {
            break;
        }

        QJsonParseError parseError;
        QJsonDocument rootDoc = QJsonDocument::fromJson(json, &parseError);
        QJsonObject rootObj   = rootDoc.object();

        QJsonArray choicesArr = rootObj.value("choices").toArray();
        if (choicesArr.isEmpty())
        {
            const QJsonObject errorObj = rootObj.value("error").toObject();
            solDebug << "errorMsg:" << errorObj.value("message");
            solDebug << "errorType:" << errorObj.value("type");
            continue;
        }

        QJsonObject choiceObj = choicesArr[0].toObject();

        QJsonValue delta = choiceObj.value("delta");
        if (delta.isUndefined())
        {
            solDebug << "not detected \'delta\'";
            continue;
        }

        QJsonValue content = delta.toObject().value("content");
        if (content.isUndefined())
        {
            if (choiceObj.value("finish_reason").toString() != "stop")
            {
                solDebug << "not detected \'content\'";
                solDebug << "last chunk:'" << event;
            }
            continue;
        }

        // contentStr += choicesArr[0].toObject()["delta"].toObject()["content"].toString();
        contentStr += content.toString();
    }

    return contentStr;
}
