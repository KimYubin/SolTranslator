// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolLogWorker.h"

#include "SolPath.h"

#include <QDebug>
#include <QMessageLogContext>
#include <QTimer>


namespace Sol
{
SolLogWorker::SolLogWorker(QObject* inParent)
    : QObject(inParent)
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


    _logFile.setFileName(SolPath::absolute(SolFile::Log));
    if (_logFile.open(QIODevice::Append | QIODevice::Text) == false)
    {
        qCritical() << "Cannot open the log file.";
        return;
    }
    _logStream.setDevice(&_logFile);
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
        _logStream << logStr << Qt::endl;
    }
    _logList.clear();

    // capacity() * 24(QString size) > 4M.
    static constexpr size_t MAX_CAPACITY = (4 * 1'000'000) / sizeof(QString);
    if (_logList.capacity() > MAX_CAPACITY)
    {
        _logList.squeeze();
    }

    _logStream.flush();
}


// ~========================
// SolLogProxy
SolLogProxy::SolLogProxy(QObject* inParent)
    : QObject(inParent)
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
} // namespace Sol
