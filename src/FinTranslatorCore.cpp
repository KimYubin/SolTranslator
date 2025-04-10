//
// Created by YubinKim on 25/03/19 수.
//

#include "FinTranslatorCore.h"

#include <QMimeData>

#include "AsyncManager.h"
#include "DataManager.h"
#include "FinTranslatorMainWidget.h"
#include "TranslateManager.h"
#include "GlobalHotKeyManager.h"


FinTranslatorCore::FinTranslatorCore(QObject* parent): QObject(parent)
{
    _dataManager         = new DataManager(this);
    _translateManager    = new TranslateManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);
    _asyncManager        = new AsyncManager(this);

    loadCache();

    _finMainWidget = new FinTranslatorMainWidget(this);
    _finMainWidget->show();
}

FinTranslatorCore::~FinTranslatorCore()
{
    asyncSaveCache();
}

void FinTranslatorCore::loadCache()
{
    // 캐시 로드
    _translateManager->updateNewCacheQueue(_dataManager->loadTranslateCache());
}

void FinTranslatorCore::asyncSaveCache()
{
    // 캐시 저장
    _dataManager->asyncSaveTranslateCache(_translateManager->getCacheQueue());
}

void FinTranslatorCore::onSimpleTranslate(const QMimeData* inMimeData)
{
    _translateManager->translateSimple(inMimeData, LangType::AUTO, LangType::ko);
}
