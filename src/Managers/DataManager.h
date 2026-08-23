// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "AbstractManager.h"
#include "Types/TextCacheKey.hpp"

namespace Sol
{
class SolTranslatorCore;

class DataManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit DataManager(SolTranslatorCore* inParent);

    cache_queue loadTranslateCache();
    bool asyncSaveTranslateCache(const cache_queue& inCacheTextQueue);

private:
    static QJsonObject convertCacheToJson(const cache_queue& inCacheTextQueue);
    cache_queue convertJsonToCache(const QJsonObject& inCacheJson);
};


} // namespace Sol

#endif //SAVEMANAGER_H
