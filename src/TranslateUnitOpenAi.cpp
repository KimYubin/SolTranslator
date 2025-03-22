//
// Created by YubinKim on 25/03/17 월.
//

#include "TranslateUnitOpenAi.h"

#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "ConfigManager.h"
#include "FinTypes.h"
#include "TranslateManager.h"

TranslateUnitOpenAI::TranslateUnitOpenAI(const TranslateRequestInfo& inTranslateRequestInfo
                                       , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void TranslateUnitOpenAI::streamingTranslate()
{
    QUrl url("https://api.openai.com/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + ConfigManager::get().getAPI()).toStdString().c_str());

    QJsonObject json;
    json["model"]  = "gpt-4o-mini";
    json["stream"] = true; // streaming

    
    QJsonArray messages;

    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = QString(StaticPrompt::OPEN_AI_PROMPT).arg(Langs::GetEnglishName(sourceLang), Langs::GetEnglishName(targetLang));
    messages.append(systemMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = originText;
    messages.append(userMessage);

    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();


    QNetworkReply* reply = post(request, data);

    connect(reply, &QIODevice::readyRead, this, [=]() { onReadyRead(reply); });
}

void TranslateUnitOpenAI::completeTranslate()
{   
    QUrl url("https://api.openai.com/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + ConfigManager::get().getAPI()).toStdString().c_str());


    QJsonObject json;
    json["model"] = "gpt-4o-mini";

    QJsonArray messages;

    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = QString(StaticPrompt::OPEN_AI_PROMPT).arg(Langs::GetEnglishName(sourceLang), Langs::GetEnglishName(targetLang));
    messages.append(systemMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = originText;
    messages.append(userMessage);

    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    post(request, data);
}

void TranslateUnitOpenAI::requestTranslate()
{
    // completeTranslate();
    streamingTranslate();
}

void TranslateUnitOpenAI::onReadyRead(QNetworkReply* reply)
{
    QByteArray chunk  = reply->readAll();
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

void TranslateUnitOpenAI::replyTranslateFinished(QNetworkReply* reply)
{
    const QByteArray responseData    = reply->readAll();
    const QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
    const QJsonObject jsonObject     = responseJson.object();
    const QJsonArray choices         = jsonObject["choices"].toArray();
    if (choices.isEmpty() == false)
    {
        const QString lastTranslatedText = choices.first().toObject()["message"].toObject()["content"].toString();
        translatedText.append(lastTranslatedText);
    }
    updateTranslatedText(translatedText);
}
