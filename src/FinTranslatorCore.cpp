// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinTranslatorCore.h"

#include <QApplication>
#include <QMimeData>

#include "FinConstants.h"
#include "FinUtilibrary.h"

#include "Managers/AsyncManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/DataManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Managers/TranslateManager.h"

#include "Managers/StyleManger.h"

#include "Widgets/FinTranslatorMainWidget.h"

FinTranslatorCore* FinTranslatorCore::_self = nullptr;

FinTranslatorCore::FinTranslatorCore(QObject* parent): QObject(parent)
{
    Q_ASSERT_X(!FinTranslatorCore::_self, "FinTranslatorCore", "there should be only one application object");
    _self = this;

    qApp->setOrganizationDomain("fin");
    qApp->setApplicationName("FinTranslator");

    QTranslator* qtTranslator = new QTranslator(this);
    if (qtTranslator->load(QLocale::system(), "fin", "_"
                        , QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        qApp->installTranslator(qtTranslator);
    }

    _dataManager         = new DataManager(this);
    _translateManager    = new TranslateManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);
    _asyncManager        = new AsyncManager(this);

    loadCache();

    StyleManger::applyTheme();
    // generate GUI widget
    _finMainWidget = new FinTranslatorMainWidget();

    // parsing
    QCommandLineParser parser;
    parser.addOption({Fin::CmdLineOptions::START_UP_RUN, "Started from Windows startup"});
    parser.process(*qApp);

    // 시작 프로그램 실행시 시스템 트레이에서 실행 
    if (parser.isSet(Fin::CmdLineOptions::START_UP_RUN))
    {
        _finMainWidget->hide();
    }
    else
    {
        _finMainWidget->show();
    }
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
    _translateManager->translateSimple(inMimeData, LangType::AUTO, finConfig.getPopupTargetLang());
}
