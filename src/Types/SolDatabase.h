// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDATABASE_H
#define SOLTRANSLATOR_SOLDATABASE_H

#include "Types/SolExpected.h"

#include <QSqlDatabase>

class QString;
class QSqlError;

class SolSql
{
public:
    explicit SolSql(const QSqlDatabase& inDB);

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
    explicit SolSqlTransactionGuard(const QSqlDatabase& inDB);

    ~SolSqlTransactionGuard();

    void transaction();
    void commit();
    void rollback();

private:
    QSqlDatabase _database;
    bool _duringTransaction;
};

#endif //SOLTRANSLATOR_SOLDATABASE_H
