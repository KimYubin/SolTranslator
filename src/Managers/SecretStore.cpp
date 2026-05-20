// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SecretStore.h"

#include "ConfigManager.h"
#include "qtkeychain/keychain.h"

#include <QVariant>

namespace
{
const QString service = "SolTranslator/";

QString serviceKey(const QString& inKey)
{
    return service + inKey;
}
} // anonymous namespace


SecretStore::SecretStore(ConfigManager* inParent)
    : QObject(inParent)
{}

void SecretStore::requestLoadSecret(const QString& inKey
                                  , LoadCallback&& inFunction)
{
    KeyCache& curCash = _cacheList[inKey];
    if (curCash.hasKey)
    {
        inFunction(curCash.secret);
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
    readJob->setKey(serviceKey(inKey));

    connect(readJob, &QKeychain::ReadPasswordJob::finished, this, [this, inKey](QKeychain::Job* inJob)
    {
        QKeychain::ReadPasswordJob* inReadJob = static_cast<QKeychain::ReadPasswordJob*>(inJob);

        QString secretStr{};
        if (inReadJob->error() == QKeychain::NoError)
        {
            secretStr = inReadJob->textData();
        }

        KeyCache& jobCash = _cacheList[inKey];
        jobCash.setCache(secretStr);

        // If there is no value, it broadcasts the completion of the task
        // by inserting an empty value.
        // The get function returns a default value if there is no value.
        for (LoadCallback& func : jobCash.callbacks)
        {
            func(jobCash.secret);
        }
        jobCash.callbacks.clear();

        if (inReadJob->error() != QKeychain::NoError
            && inReadJob->error() != QKeychain::EntryNotFound)
        {
            // todo: error process
        }
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
    writeJob->setKey(serviceKey(inKey));
    writeJob->setTextData(inValue.toString());

    connect(writeJob, &QKeychain::WritePasswordJob::finished, this, [this, jobFunc = std::move(inFunction)](QKeychain::Job* inJob) mutable
    {
        QKeychain::WritePasswordJob* inWriteJob = static_cast<QKeychain::WritePasswordJob*>(inJob);
        if (inWriteJob->error() != QKeychain::NoError)
        {
            jobFunc();
            return;
        }

        // todo: error process
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
