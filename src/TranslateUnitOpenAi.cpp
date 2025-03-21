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

TranslateUnitOpenAI::TranslateUnitOpenAI(TranslateManager* parent): TranslateUnit(parent)
{}

void TranslateUnitOpenAI::requestTranslate()
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

void TranslateUnitOpenAI::replyTranslateFinished(QNetworkReply* reply)
{
    const QByteArray responseData    = reply->readAll();
    const QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
    const QJsonObject jsonObject     = responseJson.object();
    const QJsonArray choices         = jsonObject["choices"].toArray();
    if (choices.isEmpty() == false)
    {
        const QString translatedText = choices.first().toObject()["message"].toObject()["content"].toString();

        updateTranslatedText(translatedText);
    }
}
