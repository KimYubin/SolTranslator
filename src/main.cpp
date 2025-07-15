// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include <QApplication>
#include <QFile>

#include <qdatetime.h>

#include "FinTranslatorCore.h"

// 로그 파일 스트림
QFile logFile;
QTextStream logStream;

// 메시지 핸들러 함수
void finMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
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
}

void setupLogFile()
{
    // 로그 파일 열기
    logFile.setFileName(FinPaths::getLogPath());
    if (!logFile.open(QIODevice::Append | QIODevice::Text))
    {
        qCritical() << "Cannot open the log file.";
        return;
    }
    logStream.setDevice(&logFile);

    // 메시지 핸들러 등록
    qInstallMessageHandler(finMessageHandler);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    FinTranslatorCore finTranslatorCore(&app);

    // setupLogFile();

    return app.exec();
}
