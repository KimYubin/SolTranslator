// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SecretStore.h"

#include "ConfigManager.h"
#include "qtkeychain/keychain.h"

#include "Types/SolSharedGuard.h"
#include "Utils/SolDebug.h"

#include <QVariant>

namespace Sol
{
namespace
{
const QString service = "SolTranslator";

QString serviceKey(const QString& inKey)
{
    return service + "/" + inKey;
}
} // anonymous namespace


SecretStore::SecretStore(ConfigManager* inParent)
    : QObject(inParent)
{}

void SecretStore::finishedRead(const QString& inKey, QKeychain::ReadPasswordJob* inReadJob)
{
    QString secretStr{};
    if (inReadJob->error() == QKeychain::NoError)
    {
        secretStr = inReadJob->textData();
    }

    KeyCache& jobCache = _cacheList[inKey];
    jobCache.setCache(secretStr);
    jobCache.broadcastCallbacks();

    // If there is no value, it broadcasts the completion of the task
    // by inserting an empty value.
    // The get function returns a default value if there is no value.

    if (inReadJob->error() != QKeychain::NoError
        && inReadJob->error() != QKeychain::EntryNotFound)
    {
        // todo: error handle
    }
}

void SecretStore::requestLoadSecret(const QString& inKey
                                  , LoadCallback&& inCallback)
{
    KeyCache& curCache = _cacheList[inKey];
    curCache.emplaceCallback(std::move(inCallback));

    if (curCache.hasKey)
    {
        curCache.broadcastCallbacks();
        return;
    }

    if (curCache.isLoading)
    {
        return;
    }

    curCache.isLoading = true;

    QKeychain::ReadPasswordJob* readJob = new QKeychain::ReadPasswordJob(service);
    readJob->setAutoDelete(true);
    readJob->setKey(serviceKey(inKey));

    connect(readJob, &QKeychain::ReadPasswordJob::finished, this, [this, inKey](QKeychain::Job* inJob)
    {
        Q_ASSERT_X(qobject_cast<QKeychain::ReadPasswordJob*>(inJob), "QKeychain::ReadPasswordJob::finished", "inJob is not a ReadPasswordJob");

        finishedRead(inKey, static_cast<QKeychain::ReadPasswordJob*>(inJob));
    });

    readJob->start();
}

void SecretStore::requestLoadSecretList(const std::vector<QString>& inKeyList
                                      , Callback<void()>&& inCallback)
{
    // A shared RAII guard to trigger the callback, when all secret keys are loaded.
    SolSharedGuard solSharedGuard{std::move(inCallback)};

    // Copy capture to connect the 'sharedSgg' lifetime to each 'loading'.
    for (const QString& keyStr : inKeyList)
    {
        requestLoadSecret(keyStr, [solSharedGuard](const QString&) {});
    }
}

void SecretStore::requestSaveSecret(const QString& inKey
                                  , const QVariant& inValue
                                  , Callback<void()>&& inCallback)
{
    _cacheList[inKey].setCache(inValue);

    auto writeJob = new QKeychain::WritePasswordJob(service);
    writeJob->setAutoDelete(true);
    writeJob->setKey(serviceKey(inKey));
    writeJob->setTextData(inValue.toString());

    connect(writeJob, &QKeychain::WritePasswordJob::finished, this, [this, jobCallback = std::move(inCallback)](QKeychain::Job* inJob) mutable
    {
        QKeychain::WritePasswordJob* inWriteJob = static_cast<QKeychain::WritePasswordJob*>(inJob);
        if (inWriteJob->error() == QKeychain::NoError)
        {
            jobCallback();
            return;
        }

        // todo: error handle
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

void SecretStore::KeyCache::emplaceCallback(LoadCallback&& inCallback)
{
    callbacks.emplace_back(std::move(inCallback));
}

void SecretStore::KeyCache::broadcastCallbacks()
{
    std::vector<LoadCallback> localVec = std::move(callbacks);
    for (LoadCallback& callback : localVec)
    {
        callback(secret);
    }
}
} // namespace Sol
