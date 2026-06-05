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


int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("Sol");
    QCoreApplication::setApplicationName("SolTranslator");

    SolLogHandler::setupLog();

    QApplication app(argc, argv);

    SolSingleApplication solSingleApp("SolTranslator.Single.App");
    if (solSingleApp.isAlreadyRunning())
    {
        return 0;
    }

    // solCore
    SolTranslatorCore solTranslatorCore(&app);

    // managers
    solTranslatorCore.emplaceManager<ConfigManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<TranslateManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<HistoryManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<GlobalHotKeyManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<EngineManager>(&solTranslatorCore);

    solTranslatorCore.manager<TranslateManager>()->init(
        solTranslatorCore.manager<HistoryManager>()
      , solTranslatorCore.manager<GlobalHotKeyManager>()
    );


    solTranslatorCore.postInitialize();

    QObject::connect(&solSingleApp
                   , &SolSingleApplication::raiseRequested
                   , &solTranslatorCore
                   , &SolTranslatorCore::raiseMainWidget);

    return app.exec();
}
