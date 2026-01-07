// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include <QRegularExpression>

#include "SolLog.h"
#include "SolTranslatorCore.h"

#include "Managers/HistoryManager.h"

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{
    connect(solCore->historyManager(), &HistoryManager::translateHistoryChanged, this, &HistoryModel::updateTranslateCache);
}

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
    if (index.isValid() == false || index.row() >= _translateTextCache.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case Qt::DisplayRole:
    {
        return _translateTextCache[index.row()]._translateText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
    }
    case Qt::CheckStateRole:
    {
        return _translateTextCache[index.row()]._bCheckState;
    }
    default:
        break;
    }

    return QVariant();
}

bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() == false || index.row() >= _translateTextCache.size())
    {
        return false;
    }

    switch (role)
    {
    case Qt::DisplayRole:
    {
        return true;
    }
    case Qt::CheckStateRole:
    {
        _translateTextCache[index.row()]._bCheckState = static_cast<Qt::CheckState>(value.toInt());
        return true;
    }
    default:
        break;
    }

    return false;
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex& index) const
{
    if (index.isValid() == false)
    {
        return Qt::ItemIsEnabled;
    }

    return QAbstractListModel::flags(index)
            | Qt::ItemIsEnabled
            | Qt::ItemIsUserCheckable;
}

bool HistoryModel::insertRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    // for (int row = 0; row < rows; ++row)
    //     _historyList.insert(position, {QString(), QString()});

    endInsertRows();
    return true;
}

bool HistoryModel::removeRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    // for (int row = 0; row < rows; ++row)
        // _historyList.removeAt(position);

    endRemoveRows();
    return true;
}

const TrHistoryCacheData* HistoryModel::getTranslateCache(const int inIdx) const
{
    if (0 <= inIdx && inIdx < _translateTextCache.size())
    {
        return &_translateTextCache[inIdx];
    }
    solDebug << "out of range";

    return nullptr;
}


void HistoryModel::updateTranslateCache(const std::vector<TrHistoryCacheData>& inHistoryList)
{
    beginResetModel();
    _translateTextCache = inHistoryList;
    endResetModel();
}
