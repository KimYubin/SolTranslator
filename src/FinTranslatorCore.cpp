//
// Created by YubinKim on 25/03/19 수.
//

#include "FinTranslatorCore.h"

#include <QMimeData>

#include "AsyncManager.h"
#include "DataManager.h"
#include "Widgets/FinTranslatorMainWidget.h"
#include "TranslateManager.h"
#include "GlobalHotKeyManager.h"

FinTranslatorCore* FinTranslatorCore::_self = nullptr;

FinTranslatorCore::FinTranslatorCore(QObject* parent): QObject(parent)
{
    Q_ASSERT_X(!FinTranslatorCore::_self, "FinTranslatorCore", "there should be only one application object");
    _self = this;

    qApp->setOrganizationDomain("fin");
    qApp->setApplicationName("FinTranslator");

    _dataManager         = new DataManager(this);
    _translateManager    = new TranslateManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);
    _asyncManager        = new AsyncManager(this);

    loadCache();

    _finMainWidget = new FinTranslatorMainWidget();
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
