// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SecretStore.h"

#include "qtkeychain/keychain.h"

#include <QVariant>

namespace
{
const QString service = "sol.translator.app";
} // anonymous namespace


void SecretStore::requestLoadSecret(const QString& inKey
                                  , Callback<void()>&& inFunction)
{
    KeyCache& curCash = _cacheList[inKey];
    if (curCash.hasKey)
    {
        inFunction();
        return;
    }

    curCash.callbacks.emplace_back(std::move(inFunction));

    if (curCash.isLoading)
    {
        return;
    }

    curCash.isLoading = true;

    QKeychain::ReadPasswordJob* readJob = new QKeychain::ReadPasswordJob(service);
    readJob->setAutoDelete(true);
    readJob->setKey(inKey);

    connect(readJob, &QKeychain::ReadPasswordJob::finished, this, [this, inKey](QKeychain::Job* inJob)
    {
        QKeychain::ReadPasswordJob* inReadJob = static_cast<QKeychain::ReadPasswordJob*>(inJob);
        if (inReadJob->error() != QKeychain::NoError
            || inReadJob->error() != QKeychain::EntryNotFound)
        {
            // emit error(tr("Read key failed: %1").arg(qPrintable(inReadJob->errorString())));
            return;
        }

        KeyCache& jobCash = _cacheList[inKey];
        jobCash.setCache(inReadJob->error() == QKeychain::EntryNotFound ? "" : inReadJob->textData());

        for (Callback<void()>& func : jobCash.callbacks)
        {
            func();
        }
        jobCash.callbacks.clear();
    });

    readJob->start();
}

void SecretStore::requestSaveSecret(const QString& inKey
                                  , const QVariant& inValue
                                  , Callback<void()>&& inFunction)
{
    _cacheList[inKey].setCache(inValue);

    auto writeJob = new QKeychain::WritePasswordJob(service);
    writeJob->setAutoDelete(true);
    writeJob->setKey(inKey);
    writeJob->setTextData(inValue.toString());

    connect(writeJob, &QKeychain::WritePasswordJob::finished, this, [this, jobFunc = std::move(inFunction)](QKeychain::Job* inJob) mutable
    {
        QKeychain::WritePasswordJob* inWriteJob = static_cast<QKeychain::WritePasswordJob*>(inJob);
        if (inWriteJob->error() != QKeychain::NoError)
        {
            jobFunc();
            return;
        }
        // emit error(tr("Write key failed: %1").arg(qPrintable(j->errorString())));
    });

    writeJob->start();
}

QString SecretStore::getSecret(const QString& inKey
                             , const QString& inDefault)
{
    return _cacheList[inKey].secret.isEmpty() ? inDefault : _cacheList[inKey].secret;
}

// ~===================
// SecretStore::KeyCache
void SecretStore::KeyCache::setCache(const QVariant& inValue)
{
    hasKey    = true;
    isLoading = false;
    secret    = inValue.toString();
}
