// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinPointTrUnit.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

#include "FinConstants.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

FinPointTrUnit::FinPointTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                             , TranslateManager* parent)
    : TranslateUnit(inTranslateRequestInfo, parent)
{}

void FinPointTrUnit::chatTranslate(const bool bIsStreaming)
{
    const QUrl url(bDebugMode
                       ? Fin::URLs::FIN_POINT_DEBUG
                       : Fin::URLs::FIN_POINT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonObj;
    jsonObj["sourceLang"] = Langs::GetEnglishName(_trReqData.sourceLang);
    jsonObj["targetLang"] = Langs::GetEnglishName(_trReqData.targetLang);
    jsonObj["originText"] = _trReqData.originText;
    jsonObj["bIsStream"]  = bIsStreaming; // streaming

    const QJsonDocument doc(jsonObj);
    const QByteArray data = doc.toJson();

    post(request, data, bIsStreaming);
}

void FinPointTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void FinPointTrUnit::onReadyRead()
{
    const QByteArray chunk  = _reply->readAll();
    const QString dataChunk = QString::fromUtf8(chunk);
    const QStringList lines = dataChunk.split("\n", Qt::SkipEmptyParts);

    QString cumulativeString;
    for (const QString& line : lines)
    {
        if (line.startsWith("{\"data\""))
        {
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(line.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError)
            {
                const QJsonObject jsonObj = jsonDoc.object();
                const QString jsonStr     = jsonObj["data"].toString();
                if (jsonStr != "[DONE]")
                {
                    cumulativeString += jsonStr;
                }
            }
            else
            {
                qDebug() << parseError.errorString();
            }
        }
    }

    if (cumulativeString.isEmpty() == false)
    {
        addTranslatedText(cumulativeString);
    }
}

void FinPointTrUnit::replyTranslateFinished()
{
    const QByteArray chunk  = _reply->readAll();
    const QString dataChunk = QString::fromUtf8(chunk);
    const QStringList lines = dataChunk.split("\n", Qt::SkipEmptyParts);

    if (lines.isEmpty() == false)
    {
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(lines[0].toUtf8());
        const QJsonObject jsonObj   = jsonDoc.object();
        const QString jsonStr       = jsonObj["data"].toString();

        if ((jsonStr.isEmpty() == false) && (jsonStr != "[DONE]"))
        {
            _translatedText.append(jsonStr);
        }
    }

    completeTranslatedText(_translatedText);
}
