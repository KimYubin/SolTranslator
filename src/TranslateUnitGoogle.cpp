//
// Created by YubinKim on 25/03/17 월.
//

#include "TranslateUnitGoogle.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

TranslateUnitGoogle::TranslateUnitGoogle(const TranslateRequestInfo& inTranslateRequestInfo
                                       , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void TranslateUnitGoogle::requestTranslate()
{
    QUrl url = QString("https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3").arg(
        Langs::GetCodeName(_trReqData.sourceLang)
      , Langs::GetCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.originText));

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
        const QString replyTranslatedText = jsonArr[0].toArray()[0].toArray()[0].toString();

        completeTranslatedText(replyTranslatedText);
    }
    else
    {
        qDebug() << "invalid reply";
    }
}
