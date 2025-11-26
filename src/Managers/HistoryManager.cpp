// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include "FinDatabase.h"
#include "FinLog.h"
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
        finDebug << "Could not connect to history database";
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
                              , const LangType inSourceLang
                              , const LangType inTargetLang
                              , const QString& inOriginText
                              , const QString& inTranslateText)
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
        finDebug << "not found sql files";
        return;
    }

    FinSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QVariant historyDataId;

    // insert history data
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertDataQuery);

        sqlQuery.bindValue(":engine_type", Fin::enumToQStr(inEngineType));
        sqlQuery.bindValue(":source_lang", Fin::enumToQStr(inSourceLang));
        sqlQuery.bindValue(":target_lang", Fin::enumToQStr(inTargetLang));
        sqlQuery.bindValue(":source_text", inOriginText);
        sqlQuery.bindValue(":target_text", inTranslateText);

        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
        historyDataId = sqlQuery.lastInsertId();
    }

    // insert history timeline (update)
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertTimelineQuery);

        sqlQuery.bindValue(":accessed_time", QDateTime::currentMSecsSinceEpoch());
        sqlQuery.bindValue(":history_data_id", historyDataId);

        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
    }

    markDbDirty();
    transactionGuard.commit();
}

std::tuple<bool, QString> HistoryManager::lookupHistory(const EngineType inEngineType
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

    // SQLite 버전
    const QString selectHistoryDataFilePath = ":/sql/select_history_data.sql";
    const QString insertTimelineFilePath    = ":/sql/insert_translation_timeline.sql";

    const auto [isOpenData, selectHistoryQuery]      = FinSql::readSqlFromFile(selectHistoryDataFilePath);
    const auto [isOpenTimeline, insertTimelineQuery] = FinSql::readSqlFromFile(insertTimelineFilePath);

    if ((isOpenData && isOpenTimeline) == false)
    {
        finDebug << "not found sql files";
        return res;
    }

    FinSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    qint64 historyDataId{-1};
    QString targetText;

    // find history
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(selectHistoryQuery);

        sqlQuery.bindValue(":engine_type", Fin::enumToQStr(inEngineType));
        sqlQuery.bindValue(":source_lang", Fin::enumToQStr(inSourceLang));
        sqlQuery.bindValue(":target_lang", Fin::enumToQStr(inTargetLang));
        sqlQuery.bindValue(":source_text", inOriginText);

        // error sql
        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return res;
        }
        // no history
        if (sqlQuery.first() == false)
        {
            return res;
        }

        historyDataId = sqlQuery.value(0).toLongLong();
        targetText    = sqlQuery.value(1).toString();
        res           = {true, targetText};
    }

    // insert history timeline (update)
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertTimelineQuery);

        sqlQuery.bindValue(":accessed_time", QDateTime::currentMSecsSinceEpoch());
        sqlQuery.bindValue(":history_data_id", historyDataId);

        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return res;
        }
    }

    transactionGuard.commit();
    markDbDirty();
    return res;
}

int HistoryManager::getHistoryCount()
{
    return getTranslateTextCache().size();

    FinSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    qint64 historyTimelineCount{0};

    // find history
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare("SELECT COUNT(*) FROM history_timeline;");

        // error sql
        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return 0;
        }
        // no history
        if (sqlQuery.first() == false)
        {
            return 0;
        }
        historyTimelineCount = sqlQuery.value(0).toLongLong(); 
    }

    transactionGuard.commit();
    return historyTimelineCount;
}

void HistoryManager::updateNewCacheQueue(cache_queue&& newCache)
{
    _cachingTranslateText = std::move(newCache);
}

const cache_queue& HistoryManager::getCacheQueue() const
{
    return _cachingTranslateText;
}

const std::deque<HistoryManager::trDbInfo>& HistoryManager::getTranslateTextCache()
{
    // if dirty update
    if (_bIsDirtyDB)
    {
        FinSqlTransactionGuard transactionGuard(QSqlDatabase::database());

        QSqlQuery sqlQuery;
        const QString selectTimelineFilePath = ":/sql/select_translation_timeline.sql";

        const auto [isOpenData, selectTimelineQuery] = FinSql::readSqlFromFile(selectTimelineFilePath);

        if (isOpenData == false)
        {
            finDebug << "not found sql files";
            return _translateTextCache;
        }

        sqlQuery.prepare(selectTimelineQuery);

        // error sql
        if (sqlQuery.exec() == false)
        {
            finDebug << "Error executing SQL" << sqlQuery.lastError();
            return _translateTextCache;
        }

        _translateTextCache.clear();
        while (sqlQuery.next())
        {
            _translateTextCache.emplace_back(sqlQuery.value(0).toLongLong(), sqlQuery.value(1).toString());
        }

        transactionGuard.commit();

        _bIsDirtyDB = false;
    }
    return _translateTextCache;
}

void HistoryManager::markDbDirty()
{
    _bIsDirtyDB = true;
}

