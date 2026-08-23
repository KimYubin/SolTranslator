// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "GoogleTrUnit.h"

#include "Types/ExJson.h"
#include "Utils/SolDebug.h"
#include "Utils/SolI18n.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QTextDocument>

namespace Sol
{
GoogleTrUnit::GoogleTrUnit(TranslateManager* inParent, ITranslateEngine* inEngine)
    : TranslateUnit(inParent, inEngine)
{}

void GoogleTrUnit::requestTranslateImpl()
{
    if (_trReqData.textFormat != TextStyle::PlainText)
    {
        QTextDocument txtDoc;
        txtDoc.setMarkdown(_trReqData.sourceText);
        _trReqData.sourceText = txtDoc.toPlainText();
    }

    const QUrl url = _trEngine->getDefaultUrl().arg(
        Langs::getCodeName(_trReqData.sourceLang)
      , Langs::getCodeName(_trReqData.targetLang)
      , QUrl::toPercentEncoding(_trReqData.sourceText, "()")
    ); // '()'괄호는 인코딩 대상 제외.

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



// ~======================
// GoogleEngine
GoogleEngine::GoogleEngine()
    : ITranslateEngine(EngineIds::Google)
{
    setDisplayName(i18n(Tr::GoogleTranslate));
    setDefaultUrl("https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3");
    setIconPath(":/engines/Google_Logo");
    setPriority(1);
    setTrUnitCreatorHelper<GoogleTrUnit>();
}

GoogleEngine::~GoogleEngine()
{}

namespace
{
const GoogleEngine googleEngine;
} // anonymous namespace
} // namespace Sol
