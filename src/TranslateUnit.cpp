//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "ConfigManager.h"
#include "TranslateManager.h"

TranslateUnit::TranslateUnit(Private inPrivate, QObject* parent):QNetworkAccessManager(parent)
{
    connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
}

void TranslateUnit::translateText(TranslateManager* inTranslateManager
                                  , QTextEdit* inTextEditableObj
                                  , const QString& text
                                  , const QString& sourceLang
                                  , const QString& targetLang)
{
    if (inTranslateManager == nullptr || inTextEditableObj == nullptr)
    {
        return;
    }
    parentTranslateManager = inTranslateManager;
    textEditableObject = inTextEditableObj;
    
    QUrl url("https://api.openai.com/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + ConfigManager::get().getAPI()).toStdString().c_str());
    

    QJsonObject json;
    json["model"] = "gpt-4o-mini";

    QJsonArray messages;

    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = QString("Translate from %1 to %2.").arg(sourceLang, targetLang);
    messages.append(systemMessage);

    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = text;
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
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        QJsonArray choices = jsonObject["choices"].toArray();
        if (!choices.isEmpty())
        {
            QString translatedText = choices.first().toObject()["message"].toObject()["content"].toString();
            qDebug() << "Translated Text: " << translatedText;

            textEditableObject->setText(translatedText);
        }
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();


    std::shared_ptr<TranslateUnit> self = shared_from_this();
    if (parentTranslateManager != nullptr)
    {
        parentTranslateManager->removeUnit(self);
    }
}
