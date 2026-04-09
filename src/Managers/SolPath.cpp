// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolPath.h"

#include "SolTypes.h"

#include <QDir>
#include <QStandardPaths>

namespace
{
QString solAppAbsolutePath(const QString& inSecondaryDir, const QString& inFileName)
{
    const QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (appPath.isEmpty())
    {
        qFatal() << "Cannot determine settings storage location";
        return "";
    }

    const QDir secondaryDir{appPath + "/" + inSecondaryDir + "/"};

    if (secondaryDir.exists() == false)
    {
        if (secondaryDir.mkpath(".") == false)
        {
            qFatal() << "Invalid secondary directory path: " << inSecondaryDir;
            return "";
        }
    }

    return secondaryDir.absoluteFilePath(inFileName);
}

namespace SecondaryDir
{
const QString SAVE    = "save";
const QString HISTORY = "history";
} // namespace SecondaryDir
} // anonymous namespace



SolPath::SolPath(SolTranslatorCore* parent)
    : AbstractManager(parent)
{}

QString SolPath::absolute(const SolFile inPath)
{
    struct
    {
        QString dir;
        QString file;
    } filePath;

    switch (inPath)
    {
    case SolFile::Log:
        filePath = {"logs", "log.txt"};
        break;
    case SolFile::Config:
        filePath = {SecondaryDir::SAVE, "SolConfig.ini"};
        break;
    case SolFile::TranslateHistory:
        filePath = {SecondaryDir::HISTORY, "Translate_History.json"};
        break;
    case SolFile::HistoryDB:
        filePath = {SecondaryDir::HISTORY, "Sol_Translator_History.sqlite"};
        break;

    case SolFile::Size:
        break;
    }

    return solAppAbsolutePath(filePath.dir, filePath.file);
}
