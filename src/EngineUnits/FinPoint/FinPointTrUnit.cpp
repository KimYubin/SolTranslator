// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "FinPointTrUnit.h"

#include "Types/ExJson.h"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

FinPointTrUnit::FinPointTrUnit(TranslateManager* parent, ITranslateEngine* inEngine)
    : TranslateUnit(parent, inEngine)
{}

void FinPointTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void FinPointTrUnit::chatTranslate(const bool inIsStreaming)
{
    const QUrl url(_trEngine->getDefaultUrl());
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
    const QString content = chunkToContent();
    if (content.isEmpty() == false)
    {
        appendTranslatedText(content);
    }
}

QString FinPointTrUnit::replyTranslateFinished()
{
    if (_isStream == false)
    {
        const ExJson rootJson{_reply->readAll()};
        if (const ExJson resJson = rootJson.value("data"))
        {
            const QString jsonStr = resJson.toString();
            if ((jsonStr.isEmpty() == false) && (jsonStr != "[DONE]"))
            {
                _targetText += resJson.toString();
            }
        }
        else
        {
            solDebug << resJson.error();
        }
    }

    return _targetText;
}

QString FinPointTrUnit::chunkToContent()
{
    _buffer += _reply->readAll();
    QString contentStr;

    while (_buffer.isEmpty() == false)
    {
        const int pos = _buffer.indexOf("\n\n");
        if (pos < 0)
        {
            break;
        }

        QByteArray eventJson = _buffer.left(pos);
        _buffer.remove(0, pos + 2);

        if (eventJson.startsWith("{\"data\"") == false)
        {
            solDebug << "not detected \'data\':" << eventJson;
            continue;
        }

        const ExJson rootJson{eventJson};

        // content
        const ExJson resJson = rootJson.value("data");
        if (resJson.isError())
        {
            solDebug << resJson.error();
            continue;
        }

        const QString resStr = resJson.toString();
        if (resStr == "[ERROR]")
        {
            solDebug << "last event:'" << eventJson;
            continue;
        }
        if (resStr == "[DONE]")
        {
            break;
        }

        contentStr += resStr;
    }

    return contentStr;
}


// ~======================
// FinPointEngine
FinPointEngine::FinPointEngine()
    : ITranslateEngine(EngineIds::FinPoint)
{
    setDisplayName(EngineIds::FinPoint.toString());
    setDefaultUrl("https://asia-northeast3-fintrans-33fftt.cloudfunctions.net/finpoint/text");
    setIconPath("");
    setPriority(3);
    setTrUnitCreatorHelper<FinPointTrUnit>();
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

namespace EngineIds
{
inline const EngineId FinPointDebug{"FinPointDebug"};
} // namespace EngineIds 

class FinPointEngineDebug : public ITranslateEngine
{
    Q_DISABLE_COPY_MOVE(FinPointEngineDebug)

public:
    explicit FinPointEngineDebug() : ITranslateEngine(EngineIds::FinPointDebug)
    {
        setDisplayName(EngineIds::FinPointDebug.toString());
        setDefaultUrl("http://127.0.0.1:5001/fintrans-33fftt/asia-northeast3/finpoint/text");
        setIconPath("");
        setPriority(4);
        setTrUnitCreatorHelper<FinPointTrUnit>();
    }
};

const FinPointEngineDebug finPointEngineDebug;
} // anonymous namespace

#endif
