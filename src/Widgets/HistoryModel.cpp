// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include <QRegularExpression>

#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolUtilibrary.h"

#include "Managers/HistoryManager.h"
#include "Widgets/HistoryCacheData.h"

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{
    connect(solCore->historyManager(), &HistoryManager::translateHistoryUpdated, this, &HistoryModel::updateTranslateCache);
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
    const std::expected<const HistoryCacheData*, QString> trCache = getTranslateCache(index.row());

    if (trCache.has_value() == false)
    {
        solDebug << trCache.error() << "- role :" << role;
        return QVariant();
    }

    switch (role)
    {
    case sol::DbIdRole:
    {
        return trCache.value()->getDbId();
    }
    case sol::SourceLangRole:
    {
        return trCache.value()->getSourceLang();
    }
    case sol::TagetLangRole:
    {
        return trCache.value()->getTargetLang();
    }
    case sol::SourceSimplifiedTextRole:
    {
        return trCache.value()->getSourceSimplifiedText();
    }
    case sol::TargetSimplifiedTextRole:
    {
        return trCache.value()->getTargetSimplifiedText();
    }
    case sol::SourceFullTextRole:
    {
        return trCache.value()->getSourceText();
    }
    case sol::TargetFullTextRole:
    {
        return trCache.value()->getTargetText();
    }
    case sol::TextStyleStringRole:
    {
        return sol::enumToQStr(trCache.value()->getTextStyle());
    }
    case sol::TimeStampRole:
    {
        return trCache.value()->getTimeStampString();
    }
    case sol::CheckRole:
    {
        return trCache.value()->getCheckState();
    }
    default:
    {
        break;
    }
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

std::expected<const HistoryCacheData*, QString> HistoryModel::getTranslateCache(const int inIdx) const
{
    return solCore->historyManager()->getTranslateCache(inIdx);
}


void HistoryModel::updateTranslateCache()
{
    beginResetModel();

    endResetModel();
}
