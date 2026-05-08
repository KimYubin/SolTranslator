// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMODEL_H
#define SOLTRANSLATOR_HISTORYMODEL_H

#include "Types/SolExpected.hpp"

#include <QAbstractListModel>

class HistoryCacheData;

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, const int role) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, const int role = Qt::EditRole) override;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool insertRows(const int position, const int rows, const QModelIndex& index = QModelIndex()) override;
    virtual bool removeRows(const int position, const int rows, const QModelIndex& index = QModelIndex()) override;

    Expected<const HistoryCacheData*> getHistoryCacheData(const int inIdx) const;

    Expected<const HistoryCacheData*> getHistoryCacheData(const QModelIndex& index) const;

private:
    void updateHistoryCache();

};


#endif //SOLTRANSLATOR_HISTORYMODEL_H
