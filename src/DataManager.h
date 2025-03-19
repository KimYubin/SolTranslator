//
// Created by YubinKim on 25/03/19 수.
//

#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H
#include <QObject>

#include "AbstractManager.h"
#include "FinHashQueue.h"


class DataManager : public AbstractManager
{
    Q_OBJECT

public:
    DataManager(FinTranslator* parent);

    cache_queue loadTranslateCache();
    bool saveTranslateCache(const cache_queue& CacheTextQueue);

private:
    QJsonObject convertCacheToJson(const cache_queue& CacheTextQueue);
    cache_queue convertJsonToCache(const QJsonObject& CacheJson);
};


#endif //SAVEMANAGER_H
