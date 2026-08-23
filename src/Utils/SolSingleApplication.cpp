// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSingleApplication.h"

#include "SolDebug.h"

#include <QLocalServer>
#include <QLocalSocket>

namespace Sol
{
namespace
{
const char* socketMessage = "SolTranslatorLocalServer.AlreadyRunning";
} // anonymous namespace

SolSingleApplication::SolSingleApplication(QString inAppName)
    : _appName(std::move(inAppName))
    , _localServer(nullptr)

{
    Q_ASSERT_X(_appName.isEmpty() == false, "SolSingleApplication::SolSingleApplication", "appName is empty.");

    // Check if there is a process already running.
    QLocalSocket localSocket;
    localSocket.connectToServer(_appName);
    _isRunning = localSocket.waitForConnected(500);

    if (_isRunning)
    {
        localSocket.write(socketMessage);
        localSocket.waitForBytesWritten(1000);
        return;
    }

    startServer();
}

SolSingleApplication::~SolSingleApplication()
{}

void SolSingleApplication::startServer()
{
    _localServer = new QLocalServer(this);
    if (_localServer->listen(_appName) == false)
    {
        QLocalServer::removeServer(_appName);
        if (_localServer->listen(_appName) == false)
        {
            solDebug << "Failed to run the local server. " << _localServer->errorString();
            return;
        }
    }

    connect(_localServer, &QLocalServer::newConnection, this, &SolSingleApplication::onNewConnection);
}

void SolSingleApplication::onNewConnection()
{
    if (_localServer == nullptr)
    {
        return;
    }

    QLocalSocket* clientSocket = _localServer->nextPendingConnection();
    if (clientSocket == nullptr)
    {
        return;
    }

    // 재실행 요청 확인
    connect(clientSocket, &QLocalSocket::readyRead, this, [clientSocket, this]()
    {
        if (clientSocket->readAll() == socketMessage)
        {
            emit raiseRequested();
        }

        clientSocket->deleteLater();
    });

    connect(clientSocket, &QLocalSocket::disconnected, clientSocket, &QLocalSocket::deleteLater);
}

void SolSingleApplication::onReadyRead()
{
}
} // namespace Sol
