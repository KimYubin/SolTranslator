// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SECRETKEYSTORE_H
#define SOLTRANSLATOR_SECRETKEYSTORE_H
#include "Types/SolTypes.h"

#include <QObject>

class SecretStore : public QObject
{
    Q_OBJECT

public:
    void requestLoadSecret(const QString& inKey
                         , Callback<void()>&& inFunction);
    void requestSaveSecret(const QString& inKey
                         , const QVariant& inValue
                         , Callback<void()>&& inFunction);

    QString getSecret(const QString& inKey
                    , const QString& inDefault);

private:
    struct KeyCache
    {
        void setCache(const QVariant& inValue);

        QString secret;
        std::vector<Callback<void()>> callbacks;

        bool hasKey    = false;
        bool isLoading = false;
    };

    std::unordered_map<QString, KeyCache> _cacheList;
};

#endif //SOLTRANSLATOR_SECRETKEYSTORE_H
