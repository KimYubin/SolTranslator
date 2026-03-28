// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include <QApplication>

#include "SolTranslatorCore.h"

#include "Utils/SolLogHandler.h"


int main(int argc, char* argv[])
{
    SolLogHandler::setupLog();

    QApplication app(argc, argv);

    SolTranslatorCore solTranslatorCore(&app);

    return app.exec();
}
