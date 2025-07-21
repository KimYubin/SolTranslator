// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "DataManager.h"

#include <QFile>
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QtConcurrent>

#include "FinUtilibrary.h"


#include "../../external/magic_enum.hpp"

DataManager::DataManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    
}

cache_queue DataManager::loadTranslateCache()
{
    QFile loadFile(FinPaths::getTranslateHistoryFilePath());

    if (loadFile.open(QIODevice::ReadOnly) == false)
    {
        qWarning() << "cannot open history file.";
        return {};
    }

    const QByteArray saveData = loadFile.readAll();

    const QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    return convertJsonToCache(loadDoc.object());
}

bool DataManager::asyncSaveTranslateCache(const cache_queue& CacheTextQueue)
{
    QFutureWatcher<bool>* dataWatcher = new QFutureWatcher<bool>(this);
    connect(dataWatcher, &QFutureWatcher<bool>::finished, this, [dataWatcher]
    {
        if (dataWatcher->future().result() == false)
        {
            qDebug() << "failed to save cache";
        }
        dataWatcher->deleteLater();
    });

    const QFuture<bool> future = QtConcurrent::run([CacheTextQueue]()
    {
        QFile saveFile(FinPaths::getTranslateHistoryFilePath());
        if (saveFile.open(QIODevice::WriteOnly) == false)
        {
            qWarning() << "cannot open history file.";
            return false;
        }

        const QJsonObject cacheObject = convertCacheToJson(CacheTextQueue);
        const QJsonDocument cacheDoc  = QJsonDocument(cacheObject);
        const QByteArray cacheJson    = cacheDoc.toJson();
        saveFile.write(cacheJson);
        return true;
    });

    dataWatcher->setFuture(future);

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
        cacheObject["EngineType"]    = Fin::enumToQStr(cacheKey.engineType);
        cacheObject["sourceLang"]    = Fin::enumToQStr(cacheKey.sourceLang);
        cacheObject["targetLang"]    = Fin::enumToQStr(cacheKey.targetLang);
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
            cacheKey.engineType = magic_enum::enum_cast<EngineType>(cacheObject["EngineType"].toString().toStdString()).value_or(EngineType::Default);
            cacheKey.originText = cacheObject["OriginText"].toString();
            cacheKey.sourceLang = magic_enum::enum_cast<LangType>(cacheObject["sourceLang"].toString().toStdString()).value_or(LangType::NONE);
            cacheKey.targetLang = magic_enum::enum_cast<LangType>(cacheObject["targetLang"].toString().toStdString()).value_or(LangType::NONE);

            QString cacheText = cacheObject["TranslateText"].toString();

            res.push(cacheKey, cacheText);
        }
    }
    return res;
}
