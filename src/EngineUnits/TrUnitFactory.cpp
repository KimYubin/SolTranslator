// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TrUnitFactory.h"

#include <magic_enum.hpp>

#include "EngineUnits/TranslateUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"

#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

TrUnitFactory::TrUnitFactory(QObject* parent)
{
}

TranslateUnit* TrUnitFactory::NewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                                             , TranslateManager* translateManager)
{
    TranslateUnit* trUnit = nullptr;
    const EngineType currentEngine = finConfig.getCurrentEngineType();
    switch (currentEngine)
    {
    case EngineType::Default: // break;
    case EngineType::Google:
        trUnit = new GoogleTrUnit(inTranslateRequestInfo, translateManager);
        break;
    case EngineType::OpenAI:
        trUnit = new OpenAiTrUnit(inTranslateRequestInfo, translateManager);
        break;
    case EngineType::FinPoint:
        trUnit = new FinPointTrUnit(inTranslateRequestInfo, translateManager);
        break;
    case EngineType::FinPointDebug:
    {
        FinPointTrUnit* finPointTr = new FinPointTrUnit(inTranslateRequestInfo, translateManager);
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
    
    return trUnit;
}

