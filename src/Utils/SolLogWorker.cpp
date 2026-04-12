// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolLogWorker.h"

#include "Managers/SolPath.h"

#include <QDebug>
#include <QMessageLogContext>
#include <QTimer>


SolLogWorker::SolLogWorker(QObject* parent)
    : QObject(parent)
    , _loggingTimer(nullptr)
{}

SolLogWorker::~SolLogWorker()
{
    _loggingTimer->stop();
    execLog();
}

void SolLogWorker::initialize()
{
    _loggingTimer = new QTimer(this);
    _loggingTimer->setInterval(17); // 1/60 sec
    _loggingTimer->setSingleShot(true);
    _loggingTimer->callOnTimeout(this, &SolLogWorker::execLog);


    logFile.setFileName(SolPath::absolute(SolFile::Log));
    if (logFile.open(QIODevice::Append | QIODevice::Text) == false)
    {
        qCritical() << "Cannot open the log file.";
        return;
    }
    logStream.setDevice(&logFile);
}

void SolLogWorker::enqueueLog(const QString& inLog)
{
    _logList.push_back(inLog);
    _loggingTimer->start();
}

void SolLogWorker::execLog()
{
    for (QString& logStr : _logList)
    {
        logStream << logStr << Qt::endl;
    }
    _logList.clear();

    // capacity() * 24(QString size) > 4M.
    static constexpr size_t MAX_CAPACITY = (4 * 1'000'000) / sizeof(QString);
    if (_logList.capacity() > MAX_CAPACITY)
    {
        _logList.squeeze();
    }

    logStream.flush();
}


// ~========================
// SolLogProxy
SolLogProxy::SolLogProxy(QObject* parent)
    : QObject(parent)
{
    _logWorker = new SolLogWorker();
    connect(&_workerThread, &QThread::started, _logWorker, &SolLogWorker::initialize);
    connect(&_workerThread, &QThread::finished, _logWorker, &QObject::deleteLater);
    _logWorker->moveToThread(&_workerThread);

    connect(this, &SolLogProxy::logEnqueued, _logWorker, &SolLogWorker::enqueueLog);

    _workerThread.start();
}

SolLogProxy::~SolLogProxy()
{
    _workerThread.requestInterruption();
    _workerThread.quit();
    _workerThread.wait();
}

void SolLogProxy::log(const QString& inLog)
{
     emit logEnqueued(inLog);
}
