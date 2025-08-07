// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "OpenAiTrUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "FinConstants.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

OpenAiTrUnit::OpenAiTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void OpenAiTrUnit::chatTranslate(const bool bIsStreaming)
{
    const QUrl url(Fin::URLs::OPEN_AI);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + finConfig.getAPIKey(EngineType::OpenAI)).toStdString().c_str());

    QJsonObject chatBodyJson;

    chatBodyJson["model"] = finConfig.getOpenAIModel();
    if (bIsStreaming)
    {
        chatBodyJson["stream"] = bIsStreaming; // streaming
    }
    chatBodyJson["temperature"] = finConfig.getOpenAI_Temperature();


    QJsonArray messages;

    QJsonObject developerMessage;
    developerMessage["role"] = "developer";
    developerMessage["content"] = QString(Fin::Prompt::OPEN_AI).arg(Langs::GetEnglishName(_trReqData.sourceLang), Langs::GetEnglishName(_trReqData.targetLang));
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
            QString jsonStr = line.mid(6).trimmed();
            if (jsonStr == "[DONE]")
            {
                return;
            }

            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError)
            {
                QJsonObject obj = jsonDoc.object();
                QString content = obj["choices"].toArray()[0].toObject()["delta"].toObject()["content"].toString();
                if (content.isEmpty() == false)
                {
                    addTranslatedText(content);
                }
            }
            else
            {
                qDebug() << parseError.errorString();
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
    completeTranslatedText(_translatedText);
}
