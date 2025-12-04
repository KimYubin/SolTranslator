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

QSqlError SolSql::execSQL(const QString& inFilePath)
{
    const auto [isFileOpen, sqlStr] = readSqlFromFile(inFilePath);

    if (isFileOpen == false)
    {
        solDebug << "not found sql files";
        return QSqlError("Error executing SQL", "Could not find SQL file: " + inFilePath, QSqlError::StatementError);
    }

    QSqlQuery sqlQuery(sqlStr);
    if (sqlQuery.exec() == false)
    {
        solDebug << "Error executing SQL";
        return QSqlError("Error executing SQL", "Could not execute sql: " + inFilePath, QSqlError::StatementError);
    }

    return QSqlError();
}

SolSqlTransactionGuard::SolSqlTransactionGuard(QSqlDatabase inDB)
    :_database(inDB)
{
    transaction();
}

SolSqlTransactionGuard::~SolSqlTransactionGuard()
{
    rollback();
}

void SolSqlTransactionGuard::transaction()
{
    _database.transaction();
}

void SolSqlTransactionGuard::commit()
{
    _database.commit();
}

void SolSqlTransactionGuard::rollback()
{
    _database.rollback();
}
