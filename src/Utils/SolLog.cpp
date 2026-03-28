// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolLog.h"

#include <QApplication>
#include <QFile>
#include <QTimer>

#include <qdatetime.h>
#include <qlogging.h>

#include "SolTypes.h"

namespace
{
// 로그 파일 스트림
QFile logFile;
QTextStream logStream;

QtMessageHandler originalHandler = nullptr;


// todo: 비동기 flush 필요.
// 메시지 핸들러 함수
void solMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    const QString logStr = qFormatLogMessage(type, context, msg);
    logStream << logStr << Qt::endl;

    QTimer::singleShot(0, qApp, []() { logStream.flush(); });

    // 디버그 메시지
    if (originalHandler)
    {
        originalHandler(type, context, msg);
    }
}
} // anonymous namespace


void SolLogHandler::setupLog()
{
    const QString format =
            "[%{time yy-MM-dd hh:mm:ss.zzz tt}] "
            "%{if-debug}"    "Debug"    "%{endif}"
            "%{if-info}"     "Info"     "%{endif}"
            "%{if-warning}"  "Warning"  "%{endif}"
            "%{if-critical}" "Critical" "%{endif}"
            "%{if-fatal}"    "Fatal"    "%{endif} "
            "%{file}:%{line} - %{message}";

    qSetMessagePattern(format);

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
