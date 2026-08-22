// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDATABASE_H
#define SOLTRANSLATOR_SOLDATABASE_H

#include "Types/SolExpected.hpp"

#include <QSqlDatabase>

class QString;
class QSqlError;

class SolSql
{
public:
    explicit SolSql(QSqlDatabase inDb);

    static Expected<QString> readSqlFromFile(const QString& inFilePath);
    Expected<void> execSqlFile(const QString& inFilePath);
    Expected<void> execSqlQuery(const QString& inQueryName, const QString& inQuery);

private:
    QSqlDatabase _database;

};

class SolSqlTransactionGuard
{
public:
    [[nodiscard]]
    explicit SolSqlTransactionGuard(QSqlDatabase inDb);

    ~SolSqlTransactionGuard();

    void transaction();
    void commit();
    void rollback();

private:
    QSqlDatabase _database;
    bool _duringTransaction;
};

#endif //SOLTRANSLATOR_SOLDATABASE_H
