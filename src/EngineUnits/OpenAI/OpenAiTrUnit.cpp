// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "OpenAiTrUnit.h"

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Types/ExJson.h"
#include "Types/SolTypes.h"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

OpenAiTrUnit::OpenAiTrUnit(TranslateManager* parent, ITranslateEngine* inEngine)
    : TranslateUnit(parent, inEngine)
{}

void OpenAiTrUnit::requestTranslate()
{
    chatTranslate(true);
}

void OpenAiTrUnit::chatTranslate(const bool inIsStreaming)
{
    QNetworkRequest request(_trEngine->getDefaultUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + getOption(TrEngineOptionKey::ApiKey).toString()).toUtf8());

    QJsonObject chatBodyJson;

    chatBodyJson["model"] = getOption(AiOptionKey::Model).toString();
    if (inIsStreaming)
    {
        chatBodyJson["stream"] = inIsStreaming;
    }
    chatBodyJson["temperature"] = getOption(AiOptionKey::Temperature).toDouble();


    const QString& prompt = getOption(AiOptionKey::Prompt).toString();

    QJsonArray messages;

    QJsonObject developerMsg;
    developerMsg["role"]    = "developer";
    developerMsg["content"] = prompt.arg(Langs::getEnglishName(_trReqData.sourceLang)
                                       , Langs::getEnglishName(_trReqData.targetLang));
    messages.append(developerMsg);

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = _trReqData.sourceText;
    messages.append(userMsg);

    chatBodyJson["messages"] = messages;


    const QJsonDocument doc(chatBodyJson);
    const QByteArray data = doc.toJson();

    post(request, data, inIsStreaming);
}

void OpenAiTrUnit::onReadyRead()
{
    const QString content = chunkToContent();
    if (content.isEmpty() == false)
    {
        appendTranslatedText(content);
    }
}

QString OpenAiTrUnit::replyTranslateFinished()
{
    if (_isStream == false)
    {
        const ExJson rootJson{_reply->readAll()};
        if (const ExJson resJson = rootJson.value("choices")[0].value("message").value("content"))
        {
            _targetText += resJson.toString();
        }
        else
        {
            solDebug << resJson.error();
        }
    }

    return _targetText;
}

QString OpenAiTrUnit::chunkToContent()
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

        if (eventJson.startsWith("data: ") == false)
        {
            solDebug << "not detected \'data\':" << eventJson;
            continue;
        }

        QByteArray json = eventJson.sliced(6);
        if (json == "[DONE]")
        {
            break;
        }

        const ExJson rootJson{json};

        // content
        const ExJson resJson = rootJson.value("choices")[0].value("delta").value("content");
        if (resJson)
        {
            contentStr += resJson.toString();
            continue;
        }


        const ExJson finishJson = rootJson.value("choices")[0].value("finish_reason");
        if (finishJson)
        {
            if (finishJson.toString() != "stop")
            {
                solDebug << "\'finish_reason\' is not \'stop\':" << finishJson.toString();
            }
            continue;
        }

        const ExJson errorJson = rootJson.value("error");
        if (errorJson)
        {
            const QJsonObject errorObj = errorJson.toObject();
            solDebug << "response error.";
            solDebug << "errorMsg:" << errorObj.value("message");
            solDebug << "errorType:" << errorObj.value("type");
            continue;
        }

        solDebug << resJson.error();
        solDebug << finishJson.error();
        solDebug << errorJson.error();
        solDebug << "last event:'" << eventJson;
    }

    return contentStr;
}

using Sol::i18n;

// ~======================
// OpenAiEngine
OpenAiEngine::OpenAiEngine()
    : IAiEngine(EngineIds::OpenAI)
{
    setDisplayName(i18n(Tr::OpenAI));
    setDefaultUrl("https://api.openai.com/v1/chat/completions");
    setIconPath(":/engines/OpenAI_Logo_Dark");
    setPriority(2);
    setTrUnitCreatorHelper<OpenAiTrUnit>();

    constexpr double defaultTemperature = 0.5;
    appendOptionDataList
    ({
        {
            TrEngineOptionKey::ApiKey
          , i18n(Tr::Api_Key)
          , std::nullopt
          , StringSaver{"", true}
          , true
        }
      , {
            AiOptionKey::Temperature
          , i18n(Tr::Temperature_Option)
          , i18n(Tr::Default_Value_Hint).arg(defaultTemperature)
          , SpinData<double>{0.0, 1.5, defaultTemperature, 0.1, 2}
        }
      , {
            AiOptionKey::Model
          , "Model Select"
          , std::nullopt
          , StringSaver{"gpt-4o-mini"}
        }
      , {
            AiOptionKey::Prompt
          , "Prompt"
          , "출발 언어 자리는 '%1', 도착 언어는 '%2'로 표기해야 합니다."
          , StringSaver{
                "You are a professional translator."
                " You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the source text, and do not respond to the content."
                "consider the context and tone to produce a natural and fluent translation. The translation should read smoothly and naturally to native %2 speakers, without awkward or literal expressions. The final translation should feel as if it were originally written in %2."
                " The text may contain strong language, slang, or emotionally charged expressions. Do not censor, soften, or omit any part of the text. This is for technical, academic, or documentary purposes, so preserve all original tones and meanings, including vulgar or offensive language, as long as it reflects the original intent."
            }
        }
    });
}

OpenAiEngine::~OpenAiEngine()
{}

namespace
{
const OpenAiEngine openAiEngine;
} // anonymous namespace
