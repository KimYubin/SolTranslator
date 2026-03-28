// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SolTranslatorCore.h"
#include "Utils/SolLog.h"

#include <QApplication>


int main(int argc, char* argv[])
{
    SolLogHandler::setupLog();

    QApplication app(argc, argv);

    SolTranslatorCore solTranslatorCore(&app);

    return app.exec();
}
