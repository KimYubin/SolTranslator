// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include <QMimeData>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QTextDocument>
#include <QNetworkAccessManager>

#include "AsyncManager.h"
#include "ConfigManager.h"
#include "SolTranslatorCore.h"

#include "EngineUnits/TranslateUnit.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"

#include "Widgets/PopupTranslateWidget.h"


TranslateManager::TranslateManager(SolTranslatorCore* parent): AbstractManager(parent)
{
    _networkAccessManager = new QNetworkAccessManager(this);
}

TranslateUnit* TranslateManager::executeNewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo)
{
    TranslateUnit* trUnit = nullptr;
    const EngineType currentEngine = solConfig.getCurrentEngineType();
    switch (currentEngine)
    {
    case EngineType::Default: // break;
    case EngineType::Google:
        trUnit = new GoogleTrUnit(inTranslateRequestInfo, this);
        break;
    case EngineType::OpenAI:
        trUnit = new OpenAiTrUnit(inTranslateRequestInfo, this);
        break;
    case EngineType::FinPoint:
        trUnit = new FinPointTrUnit(inTranslateRequestInfo, this);
        break;
    case EngineType::FinPointDebug:
    {
        FinPointTrUnit* finPointTr = new FinPointTrUnit(inTranslateRequestInfo, this);
        finPointTr->setDebugMode(true);
        trUnit = finPointTr;
        break;
    }
    case EngineType::Size:
        break;
    }

    // string 기반 enum과 class 매칭 유효성 검사
    bool bValid = false;
    if (const char* className = trUnit ? trUnit->metaObject()->className() : "")
    {
        if (magic_enum::enum_name(currentEngine).find(className))
        {
            bValid = true;
        }
    }
    if (bValid == false)
    {
        qDebug() << "Invalid engine type";
    }

    if (trUnit != nullptr)
    {
        trUnit->executeTextTranslation();
    }

    return trUnit;
}

QPointer<TranslateUnit> TranslateManager::translateText(const TranslateRequestInfo& inTranslateRequestInfo)
{
    TranslateUnit* transUnit = executeNewTranslateUnit(inTranslateRequestInfo);

    return QPointer<TranslateUnit>{transUnit};
}

void TranslateManager::translateAtPopup(const QMimeData* inMimeData
                                      , const LangType inSourceLang
                                      , const LangType inTargetLang)
{
    if (inMimeData->hasText() == false)
    {
        return;
    }

    PopupTranslateWidget* simple = new PopupTranslateWidget();

    auto runPopupTranslate = [this, inSourceLang, inTargetLang, simple](const QString& inOriginText, const TextStyle inTextStyle)
    {
        translateText(TranslateRequestInfo{
            simple
          , solConfig.getCurrentEngineType()
          , inOriginText
          , inTextStyle
          , inSourceLang
          , inTargetLang
          , simple
          , [=](const QString& inStr) { simple->completeTransText(inStr, inTextStyle); }
          , simple
          , [=](const QString& inStr) { simple->streamTransText(inStr, inTextStyle); }
        });
    };


    if (inMimeData->hasHtml())
    {
        AsyncManager::asyncLaunch<QString>(
            simple,
            [htmlStr = std::move(inMimeData->html())]() mutable
            {
                // list 무시하는 문법 제거.
                QTextDocument txtDoc;
                txtDoc.setHtml(htmlStr.replace(QRegularExpression(R"(list-style: none)"), ""));

                return txtDoc.toMarkdown();
            },
            [=](const QString& inMd)
            {
                runPopupTranslate(inMd, TextStyle::MarkDown);
            });
    }
    else
    {
        runPopupTranslate(inMimeData->text(), TextStyle::PlainText);
    }
}


