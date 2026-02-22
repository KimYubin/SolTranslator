// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.


#include "SolDatabase.h"

#include <QString>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#include "SolGuard.h"
#include "SolLog.h"

std::expected<QString, QString> SolSql::readSqlFromFile(const QString& inFilePath)
{
    QFile sqlFile(inFilePath);
    SolGeneralGuard<void()> fileGuard([&sqlFile]() mutable { sqlFile.close(); });

    if (sqlFile.open(QFile::ReadOnly) == false)
    {
        return std::unexpected("file open failed");
    }

    QString sqlStr = sqlFile.readAll();

    return sqlStr;
}

std::expected<void, QString> SolSql::execSQL(const QString& inFilePath)
{
    const std::expected<QString, QString> sqlStr = readSqlFromFile(inFilePath);

    if (sqlStr.has_value() == false)
    {
        return std::unexpected("Error: Could not find SQL file- " + inFilePath + " " + sqlStr.error());
    }

    QSqlQuery sqlQuery(sqlStr.value());
    if (sqlQuery.exec() == false)
    {
        return std::unexpected("Error: Could not execute sql file: " + inFilePath + " " + sqlQuery.lastError().text());
    }

    return {};
}

std::expected<void, QString> SolSql::execSqlQuery(const QString& inQueryName, const QString& inQuery)
{
    QSqlQuery sqlQuery(inQuery);
    if (sqlQuery.exec() == false)
    {
        return std::unexpected("Error: Could not execute sql query: " + inQueryName + " " + sqlQuery.lastError().text());
    }

    return {};
}

SolSqlTransactionGuard::SolSqlTransactionGuard(QSqlDatabase inDB)
    : _database(inDB)
    , duringTransaction(false)
{
    transaction();
}

SolSqlTransactionGuard::~SolSqlTransactionGuard()
{
    rollback();
}

void SolSqlTransactionGuard::transaction()
{
    if (duringTransaction)
    {
        return;
    }
    
    if (_database.transaction() == false)
    {
        solDebug << "transaction failed" << _database.lastError();
        return;
    }

    duringTransaction = true;
}

void SolSqlTransactionGuard::commit()
{
    if (duringTransaction == false)
    {
        return;
    }

    if (_database.commit() == false)
    {
        solDebug << "commit failed" << _database.lastError();
    }
    duringTransaction = false;
}

void SolSqlTransactionGuard::rollback()
{
    if (duringTransaction == false)
    {
        return;
    }

    if (_database.rollback() == false)
    {
        solDebug << "rollback failed" << _database.lastError();
    }
    duringTransaction = false;
}
