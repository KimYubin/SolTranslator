//
// Created by YubinKim on 25/03/17 월.
//

#include "TlUnitFactory.h"

#include "TranslateUnit.h"
#include "TranslateUnitOpenAi.h"

TlUnitFactory::TlUnitFactory(QObject* parent)
{
    CurrentEngine = EngineType::OpenAI;
}

TranslateUnit* TlUnitFactory::NewTranslateUnit(TranslateManager* parent)
{
    TranslateUnit* tlUnit = nullptr;
    switch (CurrentEngine)
    {
    case EngineType::None:
        break;
    case EngineType::Google:
        break;
    case EngineType::OpenAI:
        tlUnit = new TranslateUnitOpenAI(parent);
        break;
    case EngineType::Size:
        break;
    }
    
    return tlUnit;
}

void TlUnitFactory::SetEngine(EngineType NewEngine)
{
    CurrentEngine = NewEngine;
}
