// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "FinPointTrUnit.h"

#include "Types/SolConstants.h"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

FinPointTrUnit::FinPointTrUnit(TranslateManager* parent)
    : TranslateUnit(parent)
{}

void FinPointTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void FinPointTrUnit::chatTranslate(const bool inIsStreaming)
{
    const QUrl url(_isDebugMode
                       ? Sol::URLs::FIN_POINT_DEBUG
                       : Sol::URLs::FIN_POINT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonObj;
    jsonObj["sourceLang"] = Langs::getEnglishName(_trReqData.sourceLang);
    jsonObj["targetLang"] = Langs::getEnglishName(_trReqData.targetLang);
    jsonObj["originText"] = _trReqData.sourceText;
    jsonObj["bIsStream"]  = inIsStreaming; // streaming

    const QJsonDocument doc(jsonObj);
    const QByteArray data = doc.toJson();

    post(request, data, inIsStreaming);
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
                solDebug << parseError.errorString();
            }
        }
    }

    if (cumulativeString.isEmpty() == false)
    {
        appendTranslatedText(cumulativeString);
    }
}

QString FinPointTrUnit::replyTranslateFinished()
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
            _targetText.append(jsonStr);
        }
    }

    return _targetText;
}


// ~======================
// FinPointEngine
FinPointEngine::FinPointEngine()
    : ITranslateEngine(EngineIds::FinPoint)
{
    setDisplayName(EngineIds::FinPoint.toString());
    setIconPath("");
    setPriority(3);
    setTrUnitCreator([](TranslateManager* inTrManager) { return new FinPointTrUnit{inTrManager}; });
}

FinPointEngine::~FinPointEngine()
{}

namespace
{
const FinPointEngine finPointEngine;
} // anonymous namespace



#ifdef QT_DEBUG

namespace
{
class FinPointEngineDebug : public ITranslateEngine
{
    Q_DISABLE_COPY_MOVE(FinPointEngineDebug)

public:
    explicit FinPointEngineDebug() : ITranslateEngine(EngineIds::FinPointDebug)
    {
        setDisplayName(EngineIds::FinPointDebug.toString());
        setIconPath("");
        setPriority(4);
        setTrUnitCreator([](TranslateManager* inTrManager)
        {
            FinPointTrUnit* newTrUnit = new FinPointTrUnit{inTrManager};;
            newTrUnit->setDebugMode(true);
            return newTrUnit;
        });
    }

    ~FinPointEngineDebug() override
    {};
};

const FinPointEngineDebug finPointEngineDebug;
} // anonymous namespace

#endif
