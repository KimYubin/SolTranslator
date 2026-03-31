// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"
#include "Utils/SolLog.h"

#include <QApplication>


int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("Sol");
    QCoreApplication::setApplicationName("SolTranslator");

    SolLogHandler::setupLog();

    QApplication app(argc, argv);

    SolTranslatorCore solTranslatorCore(&app);

    return app.exec();
}
