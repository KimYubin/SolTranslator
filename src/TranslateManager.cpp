//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateManager.h"

#include "ConfigManager.h"
#include "TranslateUnit.h"

TranslateManager::TranslateManager(QObject* parent): QObject(parent)
{
    
}

void TranslateManager::translateText(QTextEdit* inTextEditableObj
                                     , const QString& text
                                     , const QString& sourceLang
                                     , const QString& targetLang)
{
    std::shared_ptr<TranslateUnit> tran = TranslateUnit::create(); 
    tran->translateText(this, inTextEditableObj, text, sourceLang, targetLang);

    translate_units.emplace(tran);
}

void TranslateManager::removeUnit(const std::shared_ptr<TranslateUnit>& InUnit)
{
    translate_units.erase(InUnit);
}
