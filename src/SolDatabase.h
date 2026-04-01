// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDATABASE_H
#define SOLTRANSLATOR_SOLDATABASE_H

#include <QSqlDatabase>

#include <expected>

class QString;
class QSqlError;


class SolDatabase
{
};

class SolSql
{
public:
    static std::expected<QString, QString> readSqlFromFile(const QString& inFilePath);
    static std::expected<void, QString> execSqlFile(const QString& inFilePath);
    static std::expected<void, QString> execSqlQuery(const QString& inQueryName, const QString& inQuery);
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
