// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTimer>

#include "SolDatabase.h"
#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolUtilibrary.h"

namespace
{
const char* db_type = "QSQLITE";
const char* db_connectionName = "sol_db";
} // anonymous namespace

HistoryManager::HistoryManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    initializeDB();

    _dbUpdateTimer = new QTimer(this);
    _dbUpdateTimer->setInterval(500);
    _dbUpdateTimer->setSingleShot(true);
    _dbUpdateTimer->callOnTimeout(this, &HistoryManager::applyTranslateHistory);

    markDbDirty();
}

HistoryManager::~HistoryManager()
{
    _dbUpdateTimer->stop();

    QSqlDatabase historyDB = QSqlDatabase::database();
    historyDB.close();
}

void HistoryManager::initializeDB()
{
    QSqlDatabase historyDB = QSqlDatabase::addDatabase(db_type);
    historyDB.setDatabaseName(SolPaths::getHistoryDBFilePath());
    if (historyDB.open() == false)
    {
        solDebug << "Could not connect to history database" << historyDB.lastError();
        return;
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
        const std::expected sqlExec = SolSql::execSQL(":/sql/create_" + tableName + ".sql");
        if (sqlExec.has_value() == false)
        {
            solDebug << sqlExec.error();
        }
    }

    for (QString& indexName : db_indexes)
    {
        const std::expected sqlExec = SolSql::execSQL(":/sql/create_" + indexName + ".sql");
        if (sqlExec.has_value() == false)
        {
            solDebug << sqlExec.error();
        }
    }
}

namespace
{
std::expected<void, QString> updateTimeStamp(const QVariant& inHistoryDataId)
{
    const QString insertTimelineFilePath = ":/sql/insert_translation_timeline.sql";

    const std::expected<QString, QString> insertTimelineQuery = SolSql::readSqlFromFile(insertTimelineFilePath);
    if (insertTimelineQuery.has_value() == false)
    {
        return std::unexpected(insertTimelineQuery.error() + "insert_history_timeline");
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

void HistoryManager::addHistory(const EngineType inEngineType
                              , const LangType inSourceLang
                              , const LangType inTargetLang
                              , const QString& inOriginText
                              , const QString& inTranslateText
                              , const TextStyle inTextStyle)
{
    const QString insertDataFilePath     = ":/sql/insert_translation_data.sql";
    const QString insertTimelineFilePath = ":/sql/insert_translation_timeline.sql";

    const std::expected<QString, QString> insertDataQuery = SolSql::readSqlFromFile(insertDataFilePath);

    if (insertDataQuery.has_value() == false)
    {
        solDebug << insertDataQuery.error() + "insert_history_data";
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
}

std::tuple<bool, QString> HistoryManager::lookupHistory(const EngineType inEngineType
                                                      , const QString& inOriginText
                                                      , const LangType inSourceLang
                                                      , const LangType inTargetLang)
{
    std::tuple<bool, QString> res = {false, QString()};

    const QString selectHistoryDataFilePath = ":/sql/select_history_data.sql";

    const std::expected<QString, QString> selectHistoryQuery = SolSql::readSqlFromFile(selectHistoryDataFilePath);

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

std::expected<const HistoryCacheData*, QString> HistoryManager::getTranslateCache(const int inIdx)
{
    if (inIdx < 0 || inIdx >= _translateTextCache.size())
    {
        return std::unexpected("_translateTextCache out of range");
    }

    return &_translateTextCache[inIdx];
}

bool HistoryManager::setCheckState(const int inIdx, const Qt::CheckState inState)
{
    if (inIdx < 0 || inIdx >= _translateTextCache.size())
    {
        return false;
    }

    _translateTextCache[inIdx].setCheckState(inState);
    return true;
}

void HistoryManager::applyTranslateHistory()
{
    if (_bIsDirtyDB == false)
    {
        solDebug << "DB is not dirty";
        return;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QSqlQuery sqlQuery;
    const QString selectTimelineFilePath = ":/sql/select_translation_timeline.sql";

    const std::expected<QString, QString> selectTimelineQuery = SolSql::readSqlFromFile(selectTimelineFilePath);

    if (selectTimelineQuery.has_value() == false)
    {
        solDebug << selectTimelineQuery.error() << "select_translation_timeline";
        return;
    }

    sqlQuery.prepare(selectTimelineQuery.value());
    if (sqlQuery.exec() == false)
    {
        solDebug << "Error executing SQL" << sqlQuery.lastError();
        return;
    }

    _translateTextCache.clear();
    while (sqlQuery.next())
    {
        const TextStyle textStyle = sol::qStrToEnum(sqlQuery.value(6).toString(), TextStyle::PlainText);

        _translateTextCache.emplace_back(sqlQuery.value(0).toLongLong()
                                       , sqlQuery.value(1).toString()
                                       , sqlQuery.value(2).toString()
                                       , sqlQuery.value(3).toString()
                                       , sqlQuery.value(4).toString()
                                       , sqlQuery.value(5).toString()
                                       , textStyle
                                       , sqlQuery.value(7).toLongLong());
    }

    transactionGuard.commit();

    _bIsDirtyDB = false;

    emit translateHistoryChanged();
}

void HistoryManager::markDbDirty()
{
    _bIsDirtyDB = true;
    _dbUpdateTimer->start();
}

