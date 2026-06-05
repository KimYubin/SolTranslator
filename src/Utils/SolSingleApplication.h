// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSINGLEAPPLICATION_H
#define SOLTRANSLATOR_SOLSINGLEAPPLICATION_H
#include <QObject>


class QLocalServer;

class SolSingleApplication : public QObject
{
    Q_OBJECT

public:
    explicit SolSingleApplication(QString inAppName);
    virtual ~SolSingleApplication() override;

    bool isAlreadyRunning() const { return _isRunning; };

signals:
    void raiseRequested();

private:
    /** Start the server to detect a single instance. */
    void startServer();

    void onNewConnection();
    void onReadyRead();

    QString _appName;
    QLocalServer* _localServer;
    bool _isRunning = false;
};


#endif //SOLTRANSLATOR_SOLSINGLEAPPLICATION_H
