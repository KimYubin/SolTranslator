// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLLOGWORKER_H
#define SOLTRANSLATOR_SOLLOGWORKER_H

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QThread>

class QTimer;

namespace Sol
{
class SolLogWorker : public QObject
{
    Q_OBJECT

public:
    explicit SolLogWorker(QObject* inParent = nullptr);
    ~SolLogWorker() override;

    void initialize();

public
slots :
    void enqueueLog(const QString& inLog);

private:
    void execLog();

    QStringList _logList;
    QTimer* _loggingTimer;

    // 로그 파일 스트림
    QFile _logFile;
    QTextStream _logStream;
};


class SolLogProxy : public QObject
{
    Q_OBJECT

    explicit SolLogProxy(QObject* inParent = nullptr);
    ~SolLogProxy() override;

public:
    static SolLogProxy* instance()
    {
        static SolLogProxy instance;
        return &instance;
    }

    void log(const QString& inLog);

signals:
    void logEnqueued(const QString& inLog);

private:
    QThread _workerThread;
    SolLogWorker* _logWorker;
};

} // namespace Sol

#endif //SOLTRANSLATOR_SOLLOGWORKER_H
