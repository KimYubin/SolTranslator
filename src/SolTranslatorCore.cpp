// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"

#include "Managers/StyleManger.h"
#include "Managers/TranslateManager.h"
#include "Support/WidgetInspector.h"
#include "Types/SolConstants.h"
#include "Widgets/SolMainWidget.h"

#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QTranslator>

SolTranslatorCore* SolTranslatorCore::_self = nullptr;

SolTranslatorCore::SolTranslatorCore(QObject* inParent)
    : QObject(inParent)
    , _solMainWidget(nullptr)
{
    Q_ASSERT_X(!SolTranslatorCore::_self, "SolTranslatorCore", "there should be only one sol core object");

    _self = this;

    QTranslator* qtTranslator = new QTranslator(this);
    if (qtTranslator->load(QLocale::system(), "sol", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(qtTranslator);
    }
}

SolTranslatorCore::~SolTranslatorCore()
{}


void SolTranslatorCore::postInitialize()
{
    emit postInitialized();

    // GUI setup
    StyleManger::applyTheme();

    _solMainWidget = new SolMainWidget();

    // parsing
    QCommandLineParser parser;
    parser.addOption({Sol::CmdLineOptions::START_UP_RUN, "Started from system startup"});
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

void SolTranslatorCore::raiseMainWidget()
{
    if (_solMainWidget)
    {
        _solMainWidget->showRaiseUp();
    }
}
