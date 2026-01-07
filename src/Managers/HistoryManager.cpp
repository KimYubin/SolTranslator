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

const char* db_type = "QSQLITE";
const char* db_connectionName = "sol_db";

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

QSqlError HistoryManager::initializeDB()
{
    QSqlDatabase historyDB = QSqlDatabase::addDatabase(db_type);
    historyDB.setDatabaseName(SolPaths::getHistoryDBFilePath());
    if (historyDB.open() == false)
    {
        solDebug << "Could not connect to history database";
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
        SolSql::execSQL(":/sql/create_" + tableName + ".sql");
    }
    for (QString& indexName : db_indexes)
    {
        SolSql::execSQL(":/sql/create_" + indexName + ".sql");
    }

    return QSqlError();
}

void HistoryManager::addHistory(const EngineType inEngineType
                              , const LangType inSourceLang
                              , const LangType inTargetLang
                              , const QString& inOriginText
                              , const QString& inTranslateText
                              , const TextStyle inTextStyle)
{
    const QString insertDataFilePath     = ":/sql/insert_translation_data.sql";
    const QString insertTimelineFilePath = ":/sql/insert_translation_timeline.sql";
    
    const auto [isOpenData, insertDataQuery] = SolSql::readSqlFromFile(insertDataFilePath);
    const auto [isOpenTimeline, insertTimelineQuery] = SolSql::readSqlFromFile(insertTimelineFilePath);

    if ((isOpenData && isOpenTimeline) == false)
    {
        solDebug << "not found sql files";
        return;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    QVariant historyDataId;

    // insert history data
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertDataQuery);

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

    // insert history timeline (update)
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(insertTimelineQuery);

        sqlQuery.bindValue(":accessed_time", QDateTime::currentMSecsSinceEpoch());
        sqlQuery.bindValue(":history_data_id", historyDataId);

        if (sqlQuery.exec() == false)
        {
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return;
        }
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
    const QString insertTimelineFilePath    = ":/sql/insert_translation_timeline.sql";

    const auto [isOpenData, selectHistoryQuery]      = SolSql::readSqlFromFile(selectHistoryDataFilePath);
    const auto [isOpenTimeline, insertTimelineQuery] = SolSql::readSqlFromFile(insertTimelineFilePath);

    if ((isOpenData && isOpenTimeline) == false)
    {
        solDebug << "not found sql files";
        return res;
    }

    SolSqlTransactionGuard transactionGuard(QSqlDatabase::database());

    qint64 historyDataId{-1};
    QString targetText;

    // find history
    {
        QSqlQuery sqlQuery;
        sqlQuery.prepare(selectHistoryQuery);

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
            solDebug << "Error executing SQL" << sqlQuery.lastError();
            return res;
        }
    }

    transactionGuard.commit();
    markDbDirty();
    return res;
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

    const auto [isOpenData, selectTimelineQuery] = SolSql::readSqlFromFile(selectTimelineFilePath);

    if (isOpenData == false)
    {
        solDebug << "not found sql files";
        return;
    }

    sqlQuery.prepare(selectTimelineQuery);
    if (sqlQuery.exec() == false)
    {
        solDebug << "Error executing SQL" << sqlQuery.lastError();
        return;
    }

    std::vector<TrHistoryCacheData> translateHistory;
    while (sqlQuery.next())
    {
        TextStyle textStyle = sol::qStrToEnum(sqlQuery.value(2).toString(), TextStyle::PlainText);

        translateHistory.emplace_back(sqlQuery.value(0).toLongLong()
                                    , sqlQuery.value(1).toString()
                                    , textStyle);
    }

    transactionGuard.commit();

    _bIsDirtyDB = false;

    emit translateHistoryChanged(translateHistory);
}

void HistoryManager::markDbDirty()
{
    _bIsDirtyDB = true;
    _dbUpdateTimer->start();
}

