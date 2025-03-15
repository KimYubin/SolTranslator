//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateManager.h"

#include "ConfigManager.h"
#include "FinTypes.h"
#include "simpletranslatewidget.h"
#include "TranslateUnit.h"

TranslateManager::TranslateManager(QObject* parent): QObject(parent)
{
}


void TranslateManager::translateSimple(const QString& text
                                     , const QString& sourceLang
                                     , const QString& targetLang)
{
    SimpleTranslateWidget* simple = new SimpleTranslateWidget();

    TranslateUnit* tranUnit = new TranslateUnit(this);
    tranUnit->translateText(simple, &SimpleTranslateWidget::showTranslationPopup, text, sourceLang, targetLang);
}
