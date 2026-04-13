// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"

#include "Managers/ConfigManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/StyleManger.h"
#include "Managers/TranslateManager.h"
#include "Support/WidgetInspector.h"
#include "Types/SolConstants.h"
#include "Utils/SolAsync.hpp"
#include "Widgets/SolMainWidget.h"

#include <QApplication>

SolTranslatorCore* SolTranslatorCore::_self = nullptr;

SolTranslatorCore::SolTranslatorCore(QObject* parent) : QObject(parent)
{
    Q_ASSERT_X(!SolTranslatorCore::_self, "SolTranslatorCore", "there should be only one sol core object");
    _self = this;

    // move to main.cpp
    // QCoreApplication::setOrganizationDomain("Sol");
    // QCoreApplication::setApplicationName("SolTranslator");

    QTranslator* qtTranslator = new QTranslator(this);
    if (qtTranslator->load(QLocale::system(), "sol", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        qApp->installTranslator(qtTranslator);
    }

    _configManager       = new ConfigManager(this);
    _translateManager    = new TranslateManager(this);
    _historyManager      = new HistoryManager(this);
    _globalHotKeyManager = new GlobalHotKeyManager(this);

    postInitialize();

    // GUI setup
    StyleManger::applyTheme();

    _solMainWidget = new SolMainWidget();

    // parsing
    QCommandLineParser parser;
    parser.addOption({Sol::CmdLineOptions::START_UP_RUN, "Started from Windows startup"});
    parser.process(*qApp);

    // 시작 프로그램 실행시 시스템 트레이에서 실행
    if (parser.isSet(Sol::CmdLineOptions::START_UP_RUN))
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


void SolTranslatorCore::postInitialize()
{
    emit postInitialized();
}
