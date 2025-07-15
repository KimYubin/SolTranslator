// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H
#include <QObject>

#include "AbstractManager.h"
#include "FinHashQueue.h"

class FinTranslatorCore;

class DataManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit DataManager(FinTranslatorCore* parent);

    cache_queue loadTranslateCache();
    bool asyncSaveTranslateCache(const cache_queue& CacheTextQueue);

private:
    static QJsonObject convertCacheToJson(const cache_queue& CacheTextQueue);
    cache_queue convertJsonToCache(const QJsonObject& CacheJson);
};


#endif //SAVEMANAGER_H
