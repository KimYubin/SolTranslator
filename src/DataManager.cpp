//
// Created by YubinKim on 25/03/19 수.
//

#include "DataManager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "magic_enum.hpp"

DataManager::DataManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    
}

cache_queue DataManager::loadTranslateCache()
{
    QFile loadFile(StaticPath::CACHE_QUEUE_SAVE_PATH);

    if (loadFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning("Couldn't open save file.");
        return {};
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    return convertJsonToCache(loadDoc.object());
}

bool DataManager::saveTranslateCache(const cache_queue& CacheTextQueue)
{
    QFile saveFile(StaticPath::CACHE_QUEUE_SAVE_PATH);

    if (saveFile.open(QIODevice::WriteOnly) == false)
    {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject cacheObject = convertCacheToJson(CacheTextQueue);
    saveFile.write(QJsonDocument(cacheObject).toJson());

    return true;
}

QJsonObject DataManager::convertCacheToJson(const cache_queue& CacheTextQueue)
{
    QJsonObject res;
    QJsonArray arr;

    for (auto& [cacheKey, cacheText] : CacheTextQueue)
    {
        QJsonObject cacheObject;
        cacheObject["OriginText"]    = cacheKey.originText;
        cacheObject["EngineType"]    = magic_enum::enum_name(cacheKey.engineType).data();
        cacheObject["LangType"]      = magic_enum::enum_name(cacheKey.targetLang).data();
        cacheObject["TranslateText"] = cacheText;

        arr.append(cacheObject);
    }
    res["CacheList"] = arr;
    
    return res;
}

cache_queue DataManager::convertJsonToCache(const QJsonObject& CacheJson)
{
    cache_queue res;
    QJsonValue vArr = CacheJson["CacheList"];
    if (vArr.isArray())
    {
        QJsonArray jsonArr = vArr.toArray();
        for (const QJsonValue& cacheValue : jsonArr)
        {
            QJsonObject cacheObject = cacheValue.toObject();

            TextCacheKey cacheKey;
            cacheKey.originText = cacheObject["OriginText"].toString();
            cacheKey.engineType = magic_enum::enum_cast<EngineType>(cacheObject["EngineType"].toString().toStdString()).value_or(EngineType::None);
            cacheKey.targetLang = magic_enum::enum_cast<LangType>(cacheObject["LangType"].toString().toStdString()).value_or(LangType::NONE);

            QString cacheText = cacheObject["TranslateText"].toString();

            res.push(cacheKey, cacheText);
        }
    }
    return res;
}
