// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"

#include <QApplication>
#include <QMimeData>

#include "SolConstants.h"
#include "SolUtilibrary.h"

#include "Managers/AsyncManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/DataManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "Managers/StyleManger.h"

#include "Support/WidgetInspector.h"

#include "Widgets/SolMainWidget.h"

SolTranslatorCore* SolTranslatorCore::_self = nullptr;

SolTranslatorCore::SolTranslatorCore(QObject* parent): QObject(parent)
{
    Q_ASSERT_X(!SolTranslatorCore::_self, "SolTranslatorCore", "there should be only one sol core object");
    _self = this;

    qApp->setOrganizationDomain("sol");
    qApp->setApplicationName("SolTranslator");

    QTranslator* qtTranslator = new QTranslator(this);
    if (qtTranslator->load(QLocale::system(), "sol", "_"
                        , QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        qApp->installTranslator(qtTranslator);
    }

    _dataManager         = new DataManager(this);
    _translateManager    = new TranslateManager(this);
    _historyManager      = new HistoryManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);
    _asyncManager        = new AsyncManager(this);


    StyleManger::applyTheme();
    // generate GUI widget
    _solMainWidget = new SolMainWidget();

    // parsing
    QCommandLineParser parser;
    parser.addOption({sol::CmdLineOptions::START_UP_RUN, "Started from Windows startup"});
    parser.process(*qApp);

    // 시작 프로그램 실행시 시스템 트레이에서 실행
    if (parser.isSet(sol::CmdLineOptions::START_UP_RUN))
    {
        _solMainWidget->hide();
    }
    else
    {
        _solMainWidget->show();
    }

#ifdef QT_DEBUG
    // WidgetInspector* inspector = new WidgetInspector();
#endif
}

SolTranslatorCore::~SolTranslatorCore()
{}
