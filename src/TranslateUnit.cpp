//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateUnit.h"

#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "ConfigManager.h"
#include "FinTypes.h"
#include "TranslateManager.h"

TranslateUnit::TranslateUnit(TranslateManager* parent) : QNetworkAccessManager(parent)
{
    connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
}

void TranslateUnit::translateText_Impl(const QString& inText
                                     , const QString& inSourceLang
                                     , const QString& inTargetLang)
{
    if (inText.isEmpty())
    {
        emit CompletedTranslate(inText);
        deleteLater();
        return;
    }
    if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(inText, inTargetLang);
        if (bIsFind)
        {
            emit CompletedTranslate(findCache);
            deleteLater();
            return;
        }
    }

    originText = inText;
    sourceLang = inSourceLang;
    targetLang = inTargetLang;

    QUrl url("https://api.openai.com/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + ConfigManager::get().getAPI()).toStdString().c_str());


    QJsonObject json;
    json["model"] = "gpt-4o-mini";

    QJsonArray messages;

    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = QString(StaticPrompt::OPEN_AI_PROMPT).arg(inSourceLang, inTargetLang);
    std::cout<<QString(StaticPrompt::OPEN_AI_PROMPT).arg(inSourceLang, inTargetLang).toStdString()<<std::endl;
    messages.append(systemMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = inText;
    messages.append(userMessage);

    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    post(request, data);
}

void TranslateUnit::onReplyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
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
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();

    deleteLater();
}
