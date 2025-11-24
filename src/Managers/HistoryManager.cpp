// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include "FinDatabase.h"
#include "FinTranslatorCore.h"
#include "FinUtilibrary.h"

const char* db_type = "QSQLITE";
const char* db_connectionName = "fin_db";

HistoryManager::HistoryManager(FinTranslatorCore* parent) : AbstractManager(parent)
{
    initializeDB();
}

HistoryManager::~HistoryManager()
{
    QSqlDatabase historyDB = QSqlDatabase::database();
    historyDB.close();
}

QSqlError HistoryManager::initializeDB()
{
    QSqlDatabase historyDB = QSqlDatabase::addDatabase(db_type);
    historyDB.setDatabaseName(FinPaths::getHistoryDBFilePath());
    if (historyDB.open() == false)
    {
        qDebug() << "Could not connect to history database";
        return historyDB.lastError();
    }

    QStringList db_tables = {
        "history_data"
      , "history_favorite"
      , "history_favorite_group"
      , "history_timeline"
    };
    QStringList db_indexes = {
        "index_favorite"
      , "index_timeline"
      , "index_translation_text"
    };

    for (QString& tableName : db_tables)
    {
        FinSql::execSQL(":/sql/create_" + tableName + ".sql");
    }
    for (QString& indexName : db_indexes)
    {
        FinSql::execSQL(":/sql/create_" + indexName + ".sql");
    }

    return QSqlError();
}

void HistoryManager::addHistory(const EngineType inEngineType
                              , const QString& inOriginText
                              , const QString& inTranslateText
                              , const LangType inSourceLang
                              , const LangType inTargetLang)
{
    
    // 이미 캐시되어 있다면, 순서 최신화
    _cachingTranslateText.push(TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang}, inTranslateText);
    if (_cachingTranslateText.size() > _maxCacheLength)
    {
        _cachingTranslateText.pop();
    }

    // 캐시 저장
    finCore->asyncSaveCache();


    // SQLite 버전
    const QString insertDataFilePath     = ":/sql/insert_translation_data.sql";
    const QString insertTimelineFilePath = ":/sql/insert_translation_timeline.sql";
    
    const auto [isOpenData, insertDataQuery] = FinSql::readSqlFromFile(insertDataFilePath);
    const auto [isOpenTimeline, insertTimelineQuery] = FinSql::readSqlFromFile(insertTimelineFilePath);

    if ((isOpenData && isOpenTimeline) == false)
    {
        qDebug() << "not found sql files";
        return;
    }

    FinSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QVariant historyDataId;

    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertDataQuery);

        sqlQuery.bindValue(":engine_type", Fin::enumToQStr(inEngineType));
        sqlQuery.bindValue(":source_lang", inOriginText);
        sqlQuery.bindValue(":target_lang", inTranslateText);
        sqlQuery.bindValue(":source_text", Fin::enumToQStr(inSourceLang));
        sqlQuery.bindValue(":target_text", Fin::enumToQStr(inTargetLang));

        if (sqlQuery.exec() == false)
        {
            qDebug() << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
        historyDataId = sqlQuery.lastInsertId();
    }

    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertTimelineQuery);

        sqlQuery.bindValue(":accessed_time", QDateTime::currentMSecsSinceEpoch());
        sqlQuery.bindValue(":history_data_id", historyDataId);

        if (sqlQuery.exec() == false)
        {
            qDebug() << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
    }

    transactionGuard.commit();
}

std::tuple<bool, QString> HistoryManager::findHistory(const EngineType inEngineType
                                                    , const QString& inOriginText
                                                    , const LangType inSourceLang
                                                    , const LangType inTargetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const TextCacheKey findCacheKey = TextCacheKey{inEngineType, inOriginText, inSourceLang, inTargetLang};
    if (const QString* text_cache = _cachingTranslateText.find(findCacheKey))
    {
        res = {true, *text_cache};
    }

    return res;
}

void HistoryManager::updateNewCacheQueue(cache_queue&& newCache)
{
    _cachingTranslateText = std::move(newCache);
}

const cache_queue& HistoryManager::getCacheQueue() const
{
    return _cachingTranslateText;
}
