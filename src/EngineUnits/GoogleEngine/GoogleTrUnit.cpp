// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "GoogleTrUnit.h"

#include "Types/ExJson.h"
#include "Types/SolConstants.h"
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
        txtDoc.setMarkdown(_trReqData.sourceText);
        _trReqData.sourceText = txtDoc.toPlainText();
    }

    const QUrl url = QString(Sol::URLs::GOOGLE).arg(
        Langs::getCodeName(_trReqData.sourceLang)
      , Langs::getCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.sourceText, "()")); // '()'괄호는 인코딩 대상 제외.

    const QNetworkRequest request(url);

    get(request);
}

void GoogleTrUnit::onReadyRead()
{}

QString GoogleTrUnit::replyTranslateFinished()
{
    const QByteArray chunk      = _reply->readAll();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(chunk);
    const QJsonArray jsonArr    = jsonDoc.array();
    if (jsonArr.isEmpty())
    {
        solDebug << "invalid reply";
        return {};
    }

    QString replyTargetText;
    QJsonArray translateTextArray = jsonArr[0].toArray();
    for (QJsonValueRef trTextData : translateTextArray)
    {
        replyTargetText += trTextData[0].toString();
    }

    // 출발 언어 코드
    //QString sourceLangStr = jsonArr[2].toString();
    //QLocale locale{sourceLangStr};

    return replyTargetText;
}
