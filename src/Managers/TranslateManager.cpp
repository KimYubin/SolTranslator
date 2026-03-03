// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include <QMimeData>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QTextDocument>
#include <QNetworkAccessManager>

#include "AsyncManager.h"
#include "ConfigManager.h"
#include "SolLog.h"
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

TranslateUnit* TranslateManager::executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo)
{
    TranslateUnit* trUnit = nullptr;
    const EngineType currentEngine = solConfig.getCurrentEngineType();
    switch (currentEngine)
    {
    case EngineType::Google:
        trUnit = new GoogleTrUnit(this);
        break;
    case EngineType::OpenAI:
        trUnit = new OpenAiTrUnit(this);
        break;
    case EngineType::FinPoint:
        trUnit = new FinPointTrUnit(this);
        break;
    case EngineType::FinPointDebug:
    {
        FinPointTrUnit* finPointTr = new FinPointTrUnit(this);
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
        solDebug << "Invalid engine type";
    }

    if (trUnit != nullptr)
    {
        trUnit->executeTextTranslation(std::move(inTranslateRequestInfo));
    }

    return trUnit;
}

QPointer<TranslateUnit> TranslateManager::translateText(TranslateRequestInfo&& inTranslateRequestInfo)
{
    TranslateUnit* transUnit = executeNewTranslateUnit(std::move(inTranslateRequestInfo));

    return QPointer<TranslateUnit>{transUnit};
}

void TranslateManager::translateAtPopup(const QString& inOriginText
                                      , const TextStyle inTextStyle)
{
    if (inOriginText.isEmpty())
    {
        return;
    }

    PopupTranslateWidget* popupWidget = new PopupTranslateWidget();
    popupWidget->executeTranslate(inOriginText, inTextStyle, LangType::AUTO, solConfig.getPopupTargetLang());
}


