// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "DataManager.h"

#include <QFile>
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QtConcurrent>

#include "SolLog.h"
#include "SolUtilibrary.h"


#include "../../external/magic_enum.hpp"

DataManager::DataManager(SolTranslatorCore* parent): AbstractManager(parent)
{}

cache_queue DataManager::loadTranslateCache()
{
    QFile loadFile(SolPaths::getTranslateHistoryFilePath());

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
            solDebug << "failed to save cache";
        }
        dataWatcher->deleteLater();
    });

    const QFuture<bool> future = QtConcurrent::run([CacheTextQueue]()
    {
        QFile saveFile(SolPaths::getTranslateHistoryFilePath());
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
        cacheObject["EngineType"]    = Sol::enumToQStr(cacheKey.engineType);
        cacheObject["sourceLang"]    = Sol::enumToQStr(cacheKey.sourceLang);
        cacheObject["targetLang"]    = Sol::enumToQStr(cacheKey.targetLang);
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
            cacheKey.engineType = Sol::qStrToEnum(cacheObject["EngineType"].toString(), EngineHelper::defaultEngineType());
            cacheKey.originText = cacheObject["OriginText"].toString();
            cacheKey.sourceLang = Sol::qStrToEnum(cacheObject["sourceLang"].toString(), LangType::NONE);
            cacheKey.targetLang = Sol::qStrToEnum(cacheObject["targetLang"].toString(), LangType::NONE);

            QString cacheText = cacheObject["TranslateText"].toString();

            res.push(cacheKey, cacheText);
        }
    }
    return res;
}
