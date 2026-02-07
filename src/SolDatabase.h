// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

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
    static std::expected<void, QString> execSQL(const QString& inFilePath);
};

class SolSqlTransactionGuard
{
public:
    Q_NODISCARD_CTOR
    explicit SolSqlTransactionGuard(QSqlDatabase inDB);

    ~SolSqlTransactionGuard();

    void transaction();
    void commit();
    void rollback();

private:
    QSqlDatabase _database;
    bool duringTransaction;
};

#endif //SOLTRANSLATOR_SOLDATABASE_H
