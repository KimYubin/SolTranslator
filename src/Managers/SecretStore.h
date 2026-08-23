// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SECRETKEYSTORE_H
#define SOLTRANSLATOR_SECRETKEYSTORE_H
#include "Types/SolTypes.h"

#include <QObject>

namespace QKeychain
{
class ReadPasswordJob;
}

namespace Sol
{
class ConfigManager;

using LoadCallback = Callback<void(const QString&)>;

class SecretStore : public QObject
{
    Q_OBJECT

public:
    explicit SecretStore(ConfigManager* inParent);

private:
    void finishedRead(const QString& inKey, QKeychain::ReadPasswordJob* inReadJob);

public:
    void requestLoadSecret(const QString& inKey
                         , LoadCallback&& inCallback);

    void requestLoadSecretList(const std::vector<QString>& inKeyList
                             , Callback<void()>&& inCallback);

    void requestSaveSecret(const QString& inKey
                         , const QVariant& inValue
                         , Callback<void()>&& inCallback);

    QString getSecret(const QString& inKey
                    , const QString& inDefault);

private:
    struct KeyCache
    {
        void setCache(const QVariant& inValue);
        void emplaceCallback(LoadCallback&& inCallback);
        void broadcastCallbacks();

        QString secret;
        std::vector<LoadCallback> callbacks;

        bool hasKey    = false;
        bool isLoading = false;
    };

    std::unordered_map<QString, KeyCache> _cacheList;
};

} // namespace Sol

#endif //SOLTRANSLATOR_SECRETKEYSTORE_H
