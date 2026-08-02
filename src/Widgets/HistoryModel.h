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
    explicit HistoryModel(QObject* inParent = nullptr);

    int rowCount(const QModelIndex& inParent) const override;
    int columnCount(const QModelIndex& inParent) const override;
    QVariant data(const QModelIndex& inIndex, const int inRole) const override;
    bool setData(const QModelIndex& inIndex, const QVariant& inValue, const int inRole = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& inIndex) const override;
    bool insertRows(const int inPosition, const int inRows, const QModelIndex& inIndex = QModelIndex()) override;
    bool removeRows(const int inPosition, const int inRows, const QModelIndex& inIndex = QModelIndex()) override;

    Expected<const HistoryCacheData*> getHistoryCacheData(const int inIdx) const;

    Expected<const HistoryCacheData*> getHistoryCacheData(const QModelIndex& inIndex) const;

private:
    void updateHistoryCache();

};


#endif //SOLTRANSLATOR_HISTORYMODEL_H
