//
// Created by YubinKim on 25/03/19 수.
//

#include "FinTranslatorCore.h"

#include "DataManager.h"
#include "FinTranslatorMainWidget.h"
#include "TranslateManager.h"
#include "GlobalHotKeyManager.h"


FinTranslatorCore::FinTranslatorCore(QObject* parent): QObject(parent)
{
    dataManager         = new DataManager(this);
    translateManager    = new TranslateManager(this);
    globalHotKeyManager = new GlobalHotKeyManager(this);

    // 캐시 로드
    translateManager->updateNewCacheQueue(dataManager->loadTranslateCache());

    finMainWidget = new FinTranslatorMainWidget(this);
    finMainWidget->show();
}

FinTranslatorCore::~FinTranslatorCore()
{
    // 캐시 저장
    dataManager->saveTranslateCache(translateManager->getCacheQueue());
}

void FinTranslatorCore::onSimpleTranslate(const QString& InOriginText)
{
    translateManager->translateSimple(InOriginText, LangType::AUTO, LangType::ko);
}
