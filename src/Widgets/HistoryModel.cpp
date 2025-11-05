// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include <QRegularExpression>

#include "FinTranslatorCore.h"


#include "Managers/TranslateManager.h"

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{}

HistoryModel::HistoryModel(const QList<HistoryInfo>& contacts, QObject* parent)
    : QAbstractListModel(parent), contacts(contacts)
{}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : finCore->translateManager()->getCacheQueue().size();
}   

int HistoryModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    const cache_queue& qlist = finCore->translateManager()->getCacheQueue();

    if (!index.isValid())
        return QVariant();

    if (index.row()  >= qlist.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        auto contactIt = std::prev(qlist.end());
        for (int row = 0; row < index.row() ; ++row)
        {
            contactIt = std::prev(contactIt);
        }
        const QString& str = contactIt->second;

        switch (index.column())
        {
        case 0:
            return str.left(std::min<int>(50, str.indexOf(QRegularExpression("[\\r\\n]"))));
        case 1:
            return str;
        default:
            break;
        }
    }
    return QVariant();
}

bool HistoryModel::insertRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        contacts.insert(position, {QString(), QString()});

    endInsertRows();
    return true;
}

bool HistoryModel::removeRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        contacts.removeAt(position);

    endRemoveRows();
    return true;
}

bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        const int row = index.row();
        auto contact  = contacts.value(row);

        switch (index.column())
        {
        case 0:
            contact._targetText = value.toString();
            break;
        default:
            return false;
        }
        contacts.replace(row, contact);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

const QList<HistoryInfo>& HistoryModel::getContacts() const
{
    return contacts;
}
