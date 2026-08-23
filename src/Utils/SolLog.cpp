// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolLog.h"

#include "SolLogWorker.h"
#include "SolPath.h"
#include "Types/SolTypes.h"

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QMessageLogContext>
#include <QThread>
#include <QTimer>

namespace Sol
{
namespace
{
QtMessageHandler originalHandler = nullptr;


void solMessageHandler(const QtMsgType inType, const QMessageLogContext& inContext, const QString& inMsg)
{
    // Asynchronous logging
    const QString logStr = qFormatLogMessage(inType, inContext, inMsg);
    QMetaObject::invokeMethod(SolLogProxy::instance(), &SolLogProxy::log, Qt::AutoConnection, logStr);

    // for debug message
    if (originalHandler)
    {
        originalHandler(inType, inContext, inMsg);
    }
}
} // anonymous namespace


void SolLogHandler::setupLog()
{
    // todo: Until the log file management is established, logging will be suspended in the release version .
#ifndef QT_DEBUG
    return;
#endif

    const QString format =
            // "[%{time yy-MM-dd hh:mm:ss.zzz tt}] "
            "%{if-debug}"    "Debug"    "%{endif}"
            "%{if-info}"     "Info"     "%{endif}"
            "%{if-warning}"  "Warning"  "%{endif}"
            "%{if-critical}" "Critical" "%{endif}"
            "%{if-fatal}"    "Fatal"    "%{endif} "
#ifdef QT_DEBUG
            "%{file}:%{line} - %{message}";
#else
            "%{function}:%{line} - %{message}";
#endif

    qSetMessagePattern(format);

    // Induce initialization in startup
    SolLogProxy::instance();

    // Register custom message handler
    originalHandler = qInstallMessageHandler(solMessageHandler);
}
} // namespace Sol
