// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "AbstractManager.h"
#include "Types/TextCacheKey.hpp"

class SolTranslatorCore;

class DataManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit DataManager(SolTranslatorCore* parent);

    cache_queue loadTranslateCache();
    bool asyncSaveTranslateCache(const cache_queue& CacheTextQueue);

private:
    static QJsonObject convertCacheToJson(const cache_queue& CacheTextQueue);
    cache_queue convertJsonToCache(const QJsonObject& CacheJson);
};


#endif //SAVEMANAGER_H
