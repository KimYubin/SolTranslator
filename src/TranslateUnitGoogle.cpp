//
// Created by YubinKim on 25/03/17 월.
//

#include "TranslateUnitGoogle.h"

#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

TranslateUnitGoogle::TranslateUnitGoogle(TranslateManager* parent): TranslateUnit(parent)
{}

void TranslateUnitGoogle::requestTranslate()
{
    QUrl url = QString("https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3").arg(
        Langs::GetCodeName(sourceLang)
      , Langs::GetCodeName(targetLang)
      , QUrl::toPercentEncoding(originText));

    QNetworkRequest request(url);

    get(request);
}

void TranslateUnitGoogle::replyTranslateFinished(QNetworkReply* reply)
{
    const QByteArray responseData    = reply->readAll();
    const QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
    const QJsonArray jsonArr         = responseJson.array();
    if (jsonArr.isEmpty() == false)
    {
        const QString translatedText = jsonArr[0].toArray()[0].toArray()[0].toString();

        updateTranslatedText(translatedText);
    }
    else
    {
        std::cout<<"invalid reply"<<std::endl;
    }
}
