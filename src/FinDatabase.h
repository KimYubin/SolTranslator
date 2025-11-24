// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_FINDATABASE_H
#define FINTRANSLATOR_FINDATABASE_H

#include <QSqlDatabase>

#include <utility>

class QString;
class QSqlError;


class FinDatabase
{
};

class FinSql
{
public:
    static std::pair<bool, QString> readSqlFromFile(const QString& inFilePath);
    static QSqlError execSQL(const QString& inFilePath);
};

class FinSqlTransactionGuard
{
public:
    explicit FinSqlTransactionGuard(QSqlDatabase inDB);

    ~FinSqlTransactionGuard();

    void transaction();
    void commit();
    void rollback();

private:
    QSqlDatabase _database;
};

#endif //FINTRANSLATOR_FINDATABASE_H
