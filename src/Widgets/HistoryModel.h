// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYMODEL_H
#define FINTRANSLATOR_HISTORYMODEL_H

#include <QAbstractListModel>


struct HistoryInfo
{
    QString _engine;
    QString _sourceLang;
    QString _targetLang;
    QString _sourceText;
    QString _targetText;
    
    bool operator==(const HistoryInfo& other) const
    {
        return _sourceLang == other._sourceLang && _sourceText == other._sourceText;
    }
};


class HistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject* parent = nullptr);
    explicit HistoryModel(const QList<HistoryInfo>& contacts, QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    virtual bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

    const QList<HistoryInfo>& getContacts() const;

private:
    QList<HistoryInfo> contacts;
};


#endif //FINTRANSLATOR_HISTORYMODEL_H