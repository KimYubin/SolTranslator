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
    return parent.isValid() ? 0 : solCore->historyManager()->getTranslateCacheSize();
}   

int HistoryModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    const std::expected<const TrHistoryCacheData*, QString> trCache = getTranslateCache(index.row());
    
    if (trCache.has_value() == false)
    {
        return QVariant();
    }

    switch (role)
    {
    case sol::TargetTextRole:
    {
        return trCache.value()->_translateText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
    }
    case sol::CheckRole:
    {
        return trCache.value()->_bCheckState;
    }
    default:
        break;
    }

    return QVariant();
}

bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() == false)
    {
        return false;
    }

    switch (role)
    {
    case sol::TargetTextRole:
    {
        return true;
    }
    case sol::CheckRole:
    {
        solCore->historyManager()->setCheckState(index.row(), static_cast<Qt::CheckState>(value.toInt()));
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

std::expected<const TrHistoryCacheData*, QString> HistoryModel::getTranslateCache(const int inIdx) const
{
    return solCore->historyManager()->getTranslateCache(inIdx);
}


void HistoryModel::updateTranslateCache()
{
    beginResetModel();

    endResetModel();
}
