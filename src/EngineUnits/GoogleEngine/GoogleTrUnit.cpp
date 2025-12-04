// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GoogleTrUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QTextDocument>

#include "SolConstants.h"
#include "SolLog.h"

GoogleTrUnit::GoogleTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void GoogleTrUnit::requestTranslate()
{
    if (_trReqData.textFormat != TextStyle::PlainText)
    {
        QTextDocument txtDoc;
        txtDoc.setMarkdown(_trReqData.originText);
        _trReqData.originText = txtDoc.toPlainText();
    }

    const QUrl url = QString(sol::URLs::GOOGLE).arg(
        Langs::GetCodeName(_trReqData.sourceLang)
      , Langs::GetCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.originText, "()")); // '()'괄호는 인코딩 대상 제외.

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
    if (jsonArr.isEmpty())
    {
        solDebug << "invalid reply";
        return;
    }

    QString replyTranslatedText;
    QJsonArray translateTextArray = jsonArr[0].toArray();
    for (QJsonValueRef trTextData : translateTextArray)
    {
        replyTranslatedText += trTextData.toArray()[0].toString();
    }

    // 출발 언어 코드
    //QString originLangStr = jsonArr[2].toString();
    //QLocale locale{originLangStr};

    finishTranslateRequest(replyTranslatedText);
}
