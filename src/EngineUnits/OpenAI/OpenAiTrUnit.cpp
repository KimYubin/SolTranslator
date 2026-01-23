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

OpenAiTrUnit::OpenAiTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void OpenAiTrUnit::chatTranslate(const bool bIsStreaming)
{
    const QUrl url(sol::URLs::OPEN_AI);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + solConfig.getAPIKey(EngineType::OpenAI)).toStdString().c_str());

    QJsonObject chatBodyJson;

    chatBodyJson["model"] = solConfig.getOpenAIModel();
    if (bIsStreaming)
    {
        chatBodyJson["stream"] = bIsStreaming; // streaming
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


    QJsonDocument doc(chatBodyJson);
    QByteArray data = doc.toJson();

    post(request, data, bIsStreaming);
}

void OpenAiTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void OpenAiTrUnit::onReadyRead()
{
    QByteArray chunk  = _reply->readAll();
    QString dataChunk = QString::fromUtf8(chunk);

    QStringList lines = dataChunk.split("\n", Qt::SkipEmptyParts);
    for (const QString& line : lines)
    {
        if (line.startsWith("data: "))
        {
            QString jsonStr = line.sliced(6).trimmed();
            if (jsonStr == "[DONE]")
            {
                return;
            }

            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
            if (parseError.error != QJsonParseError::NoError)
            {
                solDebug << parseError.errorString();
                return;
            }

            QJsonObject obj = jsonDoc.object();

            // choices가 없다면,
            // value는 QJsonValue(QJsonValue::Undefined)을 반환하고,
            // toArray()는 빈 Array를 반환합니다.
            QJsonArray choicesArr = obj.value("choices").toArray();
            if (choicesArr.isEmpty())
            {
                const QJsonObject errorObj = obj.value("error").toObject();
                solDebug << "openAI errorMsg:" << errorObj.value("message").toString();
                solDebug << "openAI errorType:" << errorObj.value("type").toString();
                return;
            }

            auto choices = choicesArr[0];

            QJsonValue delta = choices.toObject().value("delta");
            if (delta.isUndefined())
            {
                solDebug << "openAI not detected \'delta\'";
                return;
            }

            QJsonValue content = delta.toObject().value("content");
            if (content.isUndefined())
            {
                if (choices.toObject().value("finish_reason").toString() != "stop")
                {
                    solDebug << "openAI not detected \'content\'";
                    solDebug << "openAI last chunk:'" << chunk;
                }
                return;
            }

            QString contentStr = content.toString();

            if (contentStr.isEmpty() == false)
            {
                addTranslatedText(contentStr);
            }
        }
    }
}

void OpenAiTrUnit::replyTranslateFinished()
{
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
