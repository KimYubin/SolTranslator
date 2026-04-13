// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/TranslateManager.h"
#include "Utils/SolLog.h"

#include <QApplication>


int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("Sol");
    QCoreApplication::setApplicationName("SolTranslator");

    SolLogHandler::setupLog();

    QApplication app(argc, argv);

    // solCore
    SolTranslatorCore solTranslatorCore(&app);

    // managers
    solTranslatorCore.emplaceManager<ConfigManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<TranslateManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<HistoryManager>(&solTranslatorCore);
    solTranslatorCore.emplaceManager<GlobalHotKeyManager>(&solTranslatorCore);

    solTranslatorCore.manager<TranslateManager>()->init(
        solTranslatorCore.manager<HistoryManager>()
      , solTranslatorCore.manager<GlobalHotKeyManager>()
    );


    solTranslatorCore.postInitialize();

    return app.exec();
}
