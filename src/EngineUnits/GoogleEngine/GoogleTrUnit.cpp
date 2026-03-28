// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GoogleTrUnit.h"

#include "SolConstants.h"
#include "Utils/SolJson.h"
#include "Utils/SolLog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QTextDocument>

GoogleTrUnit::GoogleTrUnit(TranslateManager* parent)
    : TranslateUnit(parent)
{}

void GoogleTrUnit::requestTranslate()
{
    if (_trReqData.textFormat != TextStyle::PlainText)
    {
        QTextDocument txtDoc;
        txtDoc.setMarkdown(_trReqData.originText);
        _trReqData.originText = txtDoc.toPlainText();
    }

    const QUrl url = QString(Sol::URLs::GOOGLE).arg(
        Langs::getCodeName(_trReqData.sourceLang)
      , Langs::getCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.originText, "()")); // '()'괄호는 인코딩 대상 제외.

    QNetworkRequest request(url);

    get(request);
}

void GoogleTrUnit::onReadyRead()
{}

void GoogleTrUnit::replyTranslateFinished()
{
    const QByteArray chunk      = _reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(chunk);
    const QJsonArray jsonArr    = jsonDoc.array();
    if (jsonArr.isEmpty())
    {
        solDebug << "invalid reply";
        return;
    }

    QString replyTranslatedText;
    QJsonArray translateTextArray = jsonArr[0].toArray();
    for (QJsonValueRef trTextData : translateTextArray)
    {
        replyTranslatedText += trTextData[0].toString();
    }

    // 출발 언어 코드
    //QString sourceLangStr = jsonArr[2].toString();
    //QLocale locale{sourceLangStr};

    finishTranslateRequest(replyTranslatedText);
}
