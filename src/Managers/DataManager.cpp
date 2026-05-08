// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "DataManager.h"

#include "Types/TextCacheKey.hpp"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolLog.h"
#include "Utils/SolPath.h"

#include <QFile>
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QtConcurrent>

#include <magic_enum.hpp>

DataManager::DataManager(SolTranslatorCore* parent) : AbstractManager(parent)
{}

cache_queue DataManager::loadTranslateCache()
{
    QFile loadFile(SolPath::absolute(SolFile::TranslateHistory));

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
        QFile saveFile(SolPath::absolute(SolFile::TranslateHistory));
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
        cacheObject["SourceText"] = cacheKey.sourceText;
        cacheObject["EngineId"]   = cacheKey.engineId.toString();
        cacheObject["sourceLang"] = Sol::enumToQStr(cacheKey.sourceLang);
        cacheObject["targetLang"] = Sol::enumToQStr(cacheKey.targetLang);
        cacheObject["TargetText"] = cacheText;

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
            cacheKey.engineId   = EngineId{cacheObject["EngineId"].toString()};
            cacheKey.sourceText = cacheObject["SourceText"].toString();
            cacheKey.sourceLang = Sol::qStrToEnum(cacheObject["sourceLang"].toString(), LangType::NONE);
            cacheKey.targetLang = Sol::qStrToEnum(cacheObject["targetLang"].toString(), LangType::NONE);

            QString cacheText = cacheObject["TargetText"].toString();

            res.push(cacheKey, cacheText);
        }
    }
    return res;
}
