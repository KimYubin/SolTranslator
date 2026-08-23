// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "Utils/SolLog.h"
#include "Utils/SolSingleApplication.h"

#include <QApplication>
#include <QString>


int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("Sol");
    QCoreApplication::setApplicationName("SolTranslator");

    Sol::SolLogHandler::setupLog();

    QApplication app(argc, argv);

    Sol::SolSingleApplication solSingleApp("SolTranslator.Single.App");
    if (solSingleApp.isAlreadyRunning())
    {
        return 0;
    }

    // solCore
    Sol::SolTranslatorCore solTranslatorCore(&app);

    // managers
    solTranslatorCore.emplaceManager<Sol::ConfigManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<Sol::TranslateManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<Sol::HistoryManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<Sol::GlobalHotKeyManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<Sol::EngineManager>(&solTranslatorCore);

    solTranslatorCore.manager<Sol::TranslateManager>()->init(
        solTranslatorCore.manager<Sol::HistoryManager>()
      , solTranslatorCore.manager<Sol::GlobalHotKeyManager>()
    );


    solTranslatorCore.postInitialize();

    QObject::connect(&solSingleApp
                   , &Sol::SolSingleApplication::raiseRequested
                   , &solTranslatorCore
                   , &Sol::SolTranslatorCore::raiseMainWidget);

    return app.exec();
}
