// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.


#include "SolDatabase.h"

#include <QString>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

#include "SolLog.h"

std::pair<bool, QString> SolSql::readSqlFromFile(const QString& inFilePath)
{
    QFile sqlFile(inFilePath);

    if (sqlFile.open(QFile::ReadOnly) == false)
    {
        return {false, {}};
    }

    QString sqlStr = sqlFile.readAll();

    sqlFile.close();
    return {true, sqlStr};
}

std::pair<bool, QSqlError> SolSql::execSQL(const QString& inFilePath)
{
    const auto [isFileOpen, sqlStr] = readSqlFromFile(inFilePath);

    if (isFileOpen == false)
    {
        solDebug << "not found sql files";
        return {false, QSqlError("Error executing SQL", "Could not find SQL file: " + inFilePath, QSqlError::StatementError)};
    }

    QSqlQuery sqlQuery(sqlStr);
    if (sqlQuery.exec() == false)
    {
        solDebug << "Error executing SQL." << "Could not execute sql: " << inFilePath;
        return {false, sqlQuery.lastError()};
    }

    return {true, QSqlError()};
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
        solDebug << "commit failed" << _database.lastError();
    }
    duringTransaction = false;
}
