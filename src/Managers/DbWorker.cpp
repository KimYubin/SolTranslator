// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "DbWorker.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

#include "SolDatabase.h"
#include "SolLog.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include "Widgets/HistoryCacheData.h"


namespace
{
const char* db_type = "QSQLITE";
const char* db_connectionName = "sol_db";

namespace Path
{
const char* InsertTimeline      = ":/sql/insert_history_timeline.sql";
const char* InsertHistoryData   = ":/sql/insert_history_data.sql";
const char* DeleteHistoryData   = ":/sql/delete_history_data.sql";
const char* SelectHistoryData   = ":/sql/select_history_data.sql";
const char* SelectTimelineCount = ":/sql/select_timeline_count.sql";
const char* SelectTimeline      = ":/sql/select_translation_timeline.sql";
} // Path

} // anonymous namespace

DbWorker::DbWorker(QObject* parent)
    : QObject(parent)
    , _dbUpdateTimer(nullptr)
{}

DbWorker::~DbWorker()
{
    _dbUpdateTimer->stop();

    runCheckpoint(true);

    QSqlDatabase::database().close();
}

void DbWorker::initialize()
{
    initDB();

    _dbUpdateTimer = new QTimer(this);
    _dbUpdateTimer->setInterval(500);
    _dbUpdateTimer->setSingleShot(true);
    _dbUpdateTimer->callOnTimeout(this, &DbWorker::updateDbCache);

    markDbDirty();
}

void DbWorker::initDB()
{
    QSqlDatabase historyDB = QSqlDatabase::addDatabase(db_type);
    historyDB.setDatabaseName(SolPaths::getHistoryDBFilePath());
    if (historyDB.open() == false)
    {
        solDebug << "Could not connect to history database" << historyDB.lastError();
        return;
    }

    bool isValidInitDB = true;

    // To validate the remaining queries, do not stop even if an error occurs.
    auto errorLogging = [&isValidInitDB](const QString& inError)
    {
        solDebug << inError;
        isValidInitDB = false;
        return inError;
    };

    // Do not modify inside a transaction.
    SolSql::execSqlQuery("foreign_keys on", "PRAGMA foreign_keys = ON")
            .transform_error(errorLogging);
    SolSql::execSqlQuery("WAL on", "PRAGMA journal_mode = WAL")
            .transform_error(errorLogging);

    SolSqlTransactionGuard transactionGuard(historyDB);

    QStringList db_tables = {
        "history_data"
      , "history_favorite"
      , "history_favorite_group"
      , "history_timeline"
    };
    QStringList db_indexes = {
        "index_favorite"
      , "index_history_data"
      , "index_timeline_accessed_index"
      , "index_timeline_data_id"
    };

    for (QString& tableName : db_tables)
    {
        SolSql::execSqlFile(":/sql/create_" + tableName + ".sql")
                .transform_error(errorLogging);
    }

    for (QString& indexName : db_indexes)
    {
        SolSql::execSqlFile(":/sql/create_" + indexName + ".sql")
                .transform_error(errorLogging);
    }

    if (isValidInitDB == false)
    {
        return;
    }

    transactionGuard.commit();
}

namespace
{
std::expected<void, QString> updateTimeStamp(const QVariant& inHistoryDataId)
{
    const std::expected<QString, QString> insertTimelineQuery = SolSql::readSqlFromFile(Path::InsertTimeline);
    if (insertTimelineQuery.has_value() == false)
    {
        return std::unexpected(insertTimelineQuery.error() + Path::InsertTimeline);
    }

    QSqlQuery sqlQuery;
    sqlQuery.prepare(insertTimelineQuery.value());

    sqlQuery.bindValue(":accessed_time", QDateTime::currentMSecsSinceEpoch());
    sqlQuery.bindValue(":history_data_id", inHistoryDataId);

    if (sqlQuery.exec() == false)
    {
        return std::unexpected("Error executing SQL: updateTimeStamp" + sqlQuery.lastError().text());
    }

    return {};
}
} // anonymous namespace

void DbWorker::processAddHistory(const EngineType inEngineType
                               , const LangType inSourceLang
                               , const LangType inTargetLang
                               , const QString& inOriginText
                               , const QString& inTranslateText
                               , const TextStyle inTextStyle)
{
    const std::expected<QString, QString> insertDataQuery = SolSql::readSqlFromFile(Path::InsertHistoryData);

    if (insertDataQuery.has_value() == false)
    {
        solDebug << insertDataQuery.error() << Path::InsertHistoryData;
        return;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QVariant historyDataId;

    // insert history data
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertDataQuery.value());

        sqlQuery.bindValue(":engine_type", sol::enumToQStr(inEngineType));
        sqlQuery.bindValue(":source_lang", sol::enumToQStr(inSourceLang));
        sqlQuery.bindValue(":target_lang", sol::enumToQStr(inTargetLang));
        sqlQuery.bindValue(":source_text", inOriginText);
        sqlQuery.bindValue(":target_text", inTranslateText);
        sqlQuery.bindValue(":text_style",  sol::enumToQStr(inTextStyle));

        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
        historyDataId = sqlQuery.lastInsertId();
    }

    const std::expected insertRes = updateTimeStamp(historyDataId);
    if (insertRes.has_value() == false)
    {
        solDebug << insertRes.error();
        return;
    }

    transactionGuard.commit();
    markDbDirty();
    runCheckpoint();
}

void DbWorker::processDeleteHistory(const qint64 inDbId)
{
    const std::expected<QString, QString> deleteDataQuery = SolSql::readSqlFromFile(Path::DeleteHistoryData);

    if (deleteDataQuery.has_value() == false)
    {
        solDebug << deleteDataQuery.error() << Path::DeleteHistoryData;
        return;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    // delete history data
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(deleteDataQuery.value());

        sqlQuery.bindValue(":history_id", inDbId);

        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
    }

    transactionGuard.commit();
    markDbDirty();
}

void DbWorker::processLookupHistory(const EngineType inEngineType
                                  , const QString& inOriginText
                                  , const LangType inSourceLang
                                  , const LangType inTargetLang
                                  , QObject* inContext)
{
    emit lookupFinished(lookupHistoryImpl(inEngineType, inOriginText, inSourceLang, inTargetLang), inContext);
}

std::tuple<bool, QString> DbWorker::lookupHistoryImpl(const EngineType inEngineType
                                                    , const QString& inOriginText
                                                    , const LangType inSourceLang
                                                    , const LangType inTargetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const std::expected<QString, QString> selectHistoryQuery = SolSql::readSqlFromFile(Path::SelectHistoryData);
    if (selectHistoryQuery.has_value() == false)
    {
        solDebug << "not found sql files";
        return res;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QVariant historyDataId;
    QString targetText;

    // find history
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(selectHistoryQuery.value());

        sqlQuery.bindValue(":engine_type", sol::enumToQStr(inEngineType));
        sqlQuery.bindValue(":source_lang", sol::enumToQStr(inSourceLang));
        sqlQuery.bindValue(":target_lang", sol::enumToQStr(inTargetLang));
        sqlQuery.bindValue(":source_text", inOriginText);

        // error sql
        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return res;
        }
        // no history
        if (sqlQuery.first() == false)
        {
            return res;
        }

        historyDataId = sqlQuery.value(0);
        targetText    = sqlQuery.value(1).toString();
        res           = {true, targetText};
    }

    // insert history timeline (update)
    const std::expected insertRes = updateTimeStamp(historyDataId);
    if (insertRes.has_value() == false)
    {
        solDebug << insertRes.error();
        return res;
    }

    transactionGuard.commit();
    markDbDirty();
    return res;
}

void DbWorker::updateDbCache()
{
    if (_bIsDirtyDB == false)
    {
        solDebug << "DB is not dirty";
        return;
    }
    _bIsDirtyDB = false;

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    std::vector<HistoryCacheData> cacheDatas;

    // reserve vector with timeline count
    {
        const std::expected<QString, QString> timelineCountQuery = SolSql::readSqlFromFile(Path::SelectTimelineCount);

        if (timelineCountQuery.has_value() == false)
        {
            solDebug << timelineCountQuery.error() << "select_timeline_count";
            return;
        }

        QSqlQuery sqlQuery;
        sqlQuery.prepare(timelineCountQuery.value());
        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
        if (sqlQuery.next())
        {
            cacheDatas.reserve(sqlQuery.value(0).toLongLong());
        }
    }

    {
        const std::expected<QString, QString> selectTimelineQuery = SolSql::readSqlFromFile(Path::SelectTimeline);

        if (selectTimelineQuery.has_value() == false)
        {
            solDebug << selectTimelineQuery.error() << Path::SelectTimeline;
            return;
        }

        QSqlQuery sqlQuery;
        sqlQuery.prepare(selectTimelineQuery.value());
        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }

        cacheDatas.clear();
        while (sqlQuery.next())
        {
            cacheDatas.emplace_back(sqlQuery.value(0).toLongLong()
                                  , sqlQuery.value(1).toString()
                                  , sqlQuery.value(2).toString()
                                  , sqlQuery.value(3).toString()
                                  , sqlQuery.value(4).toString()
                                  , sqlQuery.value(5).toString()
                                  , sqlQuery.value(6).toLongLong()
                                  , sqlQuery.value(7).toLongLong()
                                  , sol::qStrToEnum(sqlQuery.value(8).toString(), TextStyle::PlainText));
        }
    }

    transactionGuard.commit();

    emit historyCacheUpdated(cacheDatas);
}

void DbWorker::markDbDirty()
{
    _bIsDirtyDB = true;
    _dbUpdateTimer->start();
}

void DbWorker::runCheckpoint(const bool inIsTRUNCATE)
{
    QString queryParam;
    if (inIsTRUNCATE)
    {
        queryParam = "TRUNCATE";
    }
    else
    {
        queryParam = "PASSIVE";
    }

    QSqlQuery sqlQuery;
    if (sqlQuery.exec("PRAGMA wal_checkpoint(" + queryParam + ")"))
    {
        while (sqlQuery.next())
        {
            solDebug << "complete:" << sqlQuery.value(0);
            solDebug << "modified pages:" << sqlQuery.value(1);
            solDebug << "moved pages:" << sqlQuery.value(2);
        }
    }
    else
    {
        solDebug << "run checkpoint failed" << sqlQuery.lastError();
    }
}
