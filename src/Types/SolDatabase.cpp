// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolDatabase.h"

#include "SolGuard.h"
#include "Utils/SolLog.h"

#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

SolSql::SolSql(const QSqlDatabase& inDB)
    : _database(inDB)
{}

Expected<QString> SolSql::readSqlFromFile(const QString& inFilePath)
{
    QFile sqlFile(inFilePath);
    SolGeneralGuard fileGuard{[&sqlFile]() mutable { sqlFile.close(); }};

    if (sqlFile.open(QFile::ReadOnly) == false)
    {
        return makeUnexpected("File open failed: " + sqlFile.errorString() + " File Path: " + inFilePath);
    }

    QString sqlStr = sqlFile.readAll();

    return sqlStr;
}

Expected<void> SolSql::execSqlFile(const QString& inFilePath)
{
    const Expected<QString> sqlStrExp = readSqlFromFile(inFilePath);

    if (!sqlStrExp)
    {
        return makeUnexpected(sqlStrExp.error());
    }

    QSqlQuery sqlQuery(_database);
    if (sqlQuery.exec(sqlStrExp.value()) == false)
    {
        return makeUnexpected("Error: Could not execute sql file: " + inFilePath + " " + sqlQuery.lastError().text());
    }

    return {};
}

Expected<void> SolSql::execSqlQuery(const QString& inQueryName, const QString& inQuery)
{
    QSqlQuery sqlQuery(_database);
    if (sqlQuery.exec(inQuery) == false)
    {
        return makeUnexpected("Error: Could not execute sql query: " + inQueryName + " " + sqlQuery.lastError().text());
    }

    return {};
}

SolSqlTransactionGuard::SolSqlTransactionGuard(const QSqlDatabase& inDB)
    : _database(inDB)
    , _duringTransaction(false)
{
    transaction();
}

SolSqlTransactionGuard::~SolSqlTransactionGuard()
{
    rollback();
}

void SolSqlTransactionGuard::transaction()
{
    if (_duringTransaction)
    {
        return;
    }

    if (_database.transaction() == false)
    {
        solDebug << "transaction failed" << _database.lastError();
        return;
    }

    _duringTransaction = true;
}

void SolSqlTransactionGuard::commit()
{
    if (_duringTransaction == false)
    {
        solDebug << "not during transaction";
        return;
    }

    if (_database.commit() == false)
    {
        solDebug << "commit failed" << _database.lastError();
    }
    _duringTransaction = false;
}

void SolSqlTransactionGuard::rollback()
{
    if (_duringTransaction == false)
    {
        return;
    }

    if (_database.rollback() == false)
    {
        solDebug << "rollback failed" << _database.lastError();
    }
    _duringTransaction = false;
}
