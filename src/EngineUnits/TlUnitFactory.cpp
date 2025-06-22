//
// Created by YubinKim on 25/03/17 월.
//

#include "TlUnitFactory.h"


#include "../Managers/ConfigManager.h"
#include "../magic_enum.hpp"

#include "../Managers/TranslateManager.h"
#include "TranslateUnit.h"
#include "GoogleEngine/TranslateUnitGoogle.h"
#include "OpenAI/TranslateUnitOpenAi.h"

TlUnitFactory::TlUnitFactory(QObject* parent)
{
}

TranslateUnit* TlUnitFactory::NewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                                             , TranslateManager* translateManager)
{
    TranslateUnit* tlUnit = nullptr;
    const EngineType currentEngine = ConfigManager::get().getCurrentEngineType();
    switch (currentEngine)
    {
    case EngineType::None:
        break;
    case EngineType::Google:
        tlUnit = new TranslateUnitGoogle(inTranslateRequestInfo, translateManager);
        break;
    case EngineType::OpenAI:
        tlUnit = new TranslateUnitOpenAI(inTranslateRequestInfo, translateManager);
        break;
    case EngineType::Size:
        break;
    }

    // string 기반 enum과 class 매칭 유효성 검사
    bool bValid = false;
    if (const char* className = tlUnit ? tlUnit->metaObject()->className() : nullptr)
    {
        std::string classNameSubStr = std::string(className).substr(std::size("TranslateUnit") - 1);
        if (magic_enum::enum_name(currentEngine) == classNameSubStr)
        {
            bValid = true;
        }
    }
    if (bValid == false)
    {
        qDebug() << "Invalid engine type";
    }
    
    return tlUnit;
}

