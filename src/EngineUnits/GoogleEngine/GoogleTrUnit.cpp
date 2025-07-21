// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GoogleTrUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

#include "FinConstants.h"

GoogleTrUnit::GoogleTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void GoogleTrUnit::requestTranslate()
{
    const QUrl url = QString(Fin::URLs::GOOGLE).arg(
        Langs::GetCodeName(_trReqData.sourceLang)
      , Langs::GetCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.originText, "()")); // ()괄호는 인코딩 변경 안합니다.

    QNetworkRequest request(url);

    get(request);
}

void GoogleTrUnit::onReadyRead()
{
}

void GoogleTrUnit::replyTranslateFinished()
{
    const QByteArray responseData    = _reply->readAll();
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
