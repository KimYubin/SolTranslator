//
// Created by YubinKim on 25/03/17 월.
//

#include "TlUnitFactory.h"

#include <iostream>

#include "magic_enum.hpp"
#include "TranslateUnit.h"
#include "TranslateUnitGoogle.h"
#include "TranslateUnitOpenAi.h"

TlUnitFactory::TlUnitFactory(QObject* parent)
{
    CurrentEngine = EngineType::Google;
}

TranslateUnit* TlUnitFactory::NewTranslateUnit(TranslateManager* parent)
{
    TranslateUnit* tlUnit = nullptr;
    switch (CurrentEngine)
    {
    case EngineType::None:
        break;
    case EngineType::Google:
        tlUnit = new TranslateUnitGoogle(parent);
        break;
    case EngineType::OpenAI:
        tlUnit = new TranslateUnitOpenAI(parent);
        break;
    case EngineType::Size:
        break;
    }

    // string 기반 enum과 class 매칭 유효성 검사
    bool bValid = false;
    if (const char* className = tlUnit ? tlUnit->metaObject()->className() : nullptr)
    {
        std::string classNameSubStr = std::string(className).substr(std::size("TranslateUnit") - 1);
        if (magic_enum::enum_name(CurrentEngine) == classNameSubStr)
        {
            bValid = true;
        }
    }
    if (bValid == false)
    {
        std::cout << "Invalid engine type" << std::endl;
    }
    
    return tlUnit;
}

void TlUnitFactory::SetEngine(EngineType NewEngine)
{
    CurrentEngine = NewEngine;
}
