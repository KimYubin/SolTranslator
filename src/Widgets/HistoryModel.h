// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMODEL_H
#define SOLTRANSLATOR_HISTORYMODEL_H

#include <QAbstractListModel>
#include <expected>

class HistoryCacheData;

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    virtual bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

    std::expected<const HistoryCacheData*, QString> getTranslateCache(const int inIdx) const;

private:
    void updateTranslateCache();

};


#endif //SOLTRANSLATOR_HISTORYMODEL_H
