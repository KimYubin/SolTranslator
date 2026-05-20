// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SECRETKEYSTORE_H
#define SOLTRANSLATOR_SECRETKEYSTORE_H
#include "Types/SolTypes.h"

#include <QObject>

class ConfigManager;

using LoadCallback = Callback<void(const QString&)>;

class SecretStore : public QObject
{
    Q_OBJECT

public:
    explicit SecretStore(ConfigManager* inParent);

    void requestLoadSecret(const QString& inKey
                         , LoadCallback&& inCallback);

    void requestSaveSecret(const QString& inKey
                         , const QVariant& inValue
                         , Callback<void()>&& inCallback);

    QString getSecret(const QString& inKey
                    , const QString& inDefault);

private:
    struct KeyCache
    {
        void setCache(const QVariant& inValue);

        QString secret;
        std::vector<LoadCallback> callbacks;

        bool hasKey    = false;
        bool isLoading = false;
    };

    std::unordered_map<QString, KeyCache> _cacheList;
};

#endif //SOLTRANSLATOR_SECRETKEYSTORE_H
