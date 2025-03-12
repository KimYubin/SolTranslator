//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateManager.h"

#include "ConfigManager.h"
#include "FinTypes.h"
#include "TranslateUnit.h"

TranslateManager::TranslateManager(QObject* parent): QObject(parent)
{
    
}

void TranslateManager::translateText(QTextEdit* inTextEditableObj
                                     , const QString& text
                                     , const QString& sourceLang
                                     , const QString& targetLang)
{
    TranslateUnit* tranUnit = new TranslateUnit(this);
    tranUnit->translateText(inTextEditableObj, text, Langs::ENGLISH.Name, Langs::KOREAN.Name);
}

