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
    _dataManager         = new DataManager(this);
    _translateManager    = new TranslateManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);

    // 캐시 로드
    _translateManager->updateNewCacheQueue(_dataManager->loadTranslateCache());

    _finMainWidget = new FinTranslatorMainWidget(this);
    _finMainWidget->show();
}

FinTranslatorCore::~FinTranslatorCore()
{
    // 캐시 저장
    _dataManager->saveTranslateCache(_translateManager->getCacheQueue());
}

void FinTranslatorCore::onSimpleTranslate(const QString& InOriginText)
{
    _translateManager->translateSimple(InOriginText, LangType::AUTO, LangType::ko);
}
