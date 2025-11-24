// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.


#include "FinDatabase.h"

#include <QString>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>

std::pair<bool, QString> FinSql::readSqlFromFile(const QString& inFilePath)
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

QSqlError FinSql::execSQL(const QString& inFilePath)
{
    const auto [isFileOpen, sqlStr] = readSqlFromFile(inFilePath);

    if (isFileOpen == false)
    {
        qDebug() << "not found sql files";
        return QSqlError("Error executing SQL", "Could not find SQL file: " + inFilePath, QSqlError::StatementError);
    }

    QSqlQuery sqlQuery(sqlStr);
    if (sqlQuery.exec() == false)
    {
        qDebug() << "Error executing SQL";
        return QSqlError("Error executing SQL", "Could not execute sql: " + inFilePath, QSqlError::StatementError);
    }

    return QSqlError();
}

FinSqlTransactionGuard::FinSqlTransactionGuard(QSqlDatabase inDB)
    :_database(inDB)
{
    transaction();
}

FinSqlTransactionGuard::~FinSqlTransactionGuard()
{
    rollback();
}

void FinSqlTransactionGuard::transaction()
{
    _database.transaction();
}

void FinSqlTransactionGuard::commit()
{
    _database.commit();
}

void FinSqlTransactionGuard::rollback()
{
    _database.rollback();
}
