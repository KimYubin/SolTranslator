// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include <QRegularExpression>

#include "FinTranslatorCore.h"


#include "Managers/HistoryManager.h"

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{
    connect(finCore->historyManager(), &HistoryManager::translateHistoryChanged, this, &HistoryModel::resetModel);
    resetModel();
}

HistoryModel::HistoryModel(const QList<HistoryInfo>& contacts, QObject* parent)
    : QAbstractListModel(parent), _historyList(contacts)
{}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : _translateTextCache.size();
}   

int HistoryModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _translateTextCache.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        return _translateTextCache[index.row()]._translateText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
    }
    return QVariant();
}

bool HistoryModel::insertRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        _historyList.insert(position, {QString(), QString()});

    endInsertRows();
    return true;
}

bool HistoryModel::removeRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        _historyList.removeAt(position);

    endRemoveRows();
    return true;
}

bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        const int row = index.row();
        auto contact  = _historyList.value(row);

        switch (index.column())
        {
        case 0:
            contact._targetText = value.toString();
            break;
        default:
            return false;
        }
        _historyList.replace(row, contact);
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

const QList<HistoryInfo>& HistoryModel::getHistoryList() const
{
    return _historyList;
}

void HistoryModel::resetModel()
{
    beginResetModel();
    _translateTextCache = finCore->historyManager()->getTranslateTextCache();
    endResetModel();
}
