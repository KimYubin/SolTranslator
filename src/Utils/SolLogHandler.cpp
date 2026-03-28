// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolLogHandler.h"

#include <QApplication>
#include <QFile>

#include <qdatetime.h>
#include <qlogging.h>

#include "SolTypes.h"

namespace
{
// 로그 파일 스트림
QFile logFile;
QTextStream logStream;

QtMessageHandler originalHandler = nullptr;


// 메시지 핸들러 함수
void solMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString logType;

    switch (type)
    {
    case QtMsgType::QtDebugMsg:
        logType = "Debug";
        break;
    case QtMsgType::QtInfoMsg:
        logType = "Info";
        break;
    case QtMsgType::QtWarningMsg:
        logType = "Warning";
        break;
    case QtMsgType::QtCriticalMsg:
        logType = "Critical";
        break;
    case QtMsgType::QtFatalMsg:
        logType = "Fatal";
        break;
    }

    const QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    const QString logStr    = QString("[%1] %2: %3").arg(timeStamp, logType, msg);

    logStream << logStr << Qt::endl;
    logStream.flush();

    // 디버그 메시지
    if (originalHandler)
    {
        originalHandler(type, context, msg);
    }
}
} // anonymous namespace


void SolLogHandler::setupLog()
{
    logFile.setFileName(SolPaths::getLogPath());
    if (logFile.open(QIODevice::Append | QIODevice::Text) == false)
    {
        qCritical() << "Cannot open the log file.";
        return;
    }
    logStream.setDevice(&logFile);

    // 메시지 핸들러 등록
    originalHandler = qInstallMessageHandler(solMessageHandler);
}