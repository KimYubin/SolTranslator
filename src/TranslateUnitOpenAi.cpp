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
{
}

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
    systemMessage["content"] = QString(StaticPrompt::OPEN_AI_PROMPT).arg(sourceLang, targetLang);
    std::cout<<QString(StaticPrompt::OPEN_AI_PROMPT).arg(sourceLang, targetLang).toStdString()<<std::endl;
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

void TranslateUnitOpenAI::replyTranslate(QNetworkReply* reply)
{
    QByteArray responseData    = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject     = jsonResponse.object();
    QJsonArray choices         = jsonObject["choices"].toArray();
    if (!choices.isEmpty())
    {
        const QString translatedText = choices.first().toObject()["message"].toObject()["content"].toString();

        if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
        {
            translate_manager->setCacheText(originText, translatedText, targetLang);
        }

        emit CompletedTranslate(translatedText);
    }
}
