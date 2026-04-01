// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include "SolTranslatorCore.h"
#include "SolUtilibrary.h"
#include "Managers/HistoryManager.h"
#include "Utils/SolLog.h"
#include "Widgets/HistoryCacheData.h"

#include <QRegularExpression>

HistoryModel::HistoryModel(QObject* parent)
    : QAbstractListModel(parent)
{
    connect(solCore->historyManager(), &HistoryManager::translateHistoryUpdated, this, &HistoryModel::updateHistoryCache);
}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : solCore->historyManager()->getHistoryCacheSize();
}

int HistoryModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant HistoryModel::data(const QModelIndex& index, const int role) const
{
    const std::expected<const HistoryCacheData*, QString> trCache = getHistoryCacheData(index.row());

    if (trCache.has_value() == false)
    {
        solDebug << trCache.error() << "- role :" << role;
        return QVariant();
    }

    switch (role)
    {
    case Sol::DbIdRole:
    {
        return trCache.value()->getDbId();
    }
    case Sol::SourceLangRole:
    {
        return trCache.value()->getSourceLang();
    }
    case Sol::TagetLangRole:
    {
        return trCache.value()->getTargetLang();
    }
    case Sol::SourceSimplifiedTextRole:
    {
        return trCache.value()->getSourceSimplifiedText();
    }
    case Sol::TargetSimplifiedTextRole:
    {
        return trCache.value()->getTargetSimplifiedText();
    }
    case Sol::SourceFullTextRole:
    {
        return trCache.value()->getSourceText();
    }
    case Sol::TargetFullTextRole:
    {
        return trCache.value()->getTargetText();
    }
    case Sol::TextStyleStringRole:
    {
        return Sol::enumToQStr(trCache.value()->getTextStyle());
    }
    case Sol::TimeStampRole:
    {
        return trCache.value()->getTimeStampString();
    }
    case Sol::CheckRole:
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

bool HistoryModel::setData(const QModelIndex& index, const QVariant& value, const int role)
{
    if (index.isValid() == false)
    {
        return false;
    }

    switch (role)
    {
    case Sol::CheckRole:
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

bool HistoryModel::insertRows(const int position, const int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    // for (int row = 0; row < rows; ++row)
    //     _historyList.insert(position, {QString(), QString()});

    endInsertRows();
    return true;
}

bool HistoryModel::removeRows(const int position, const int rows, const QModelIndex& index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    // for (int row = 0; row < rows; ++row)
        // _historyList.removeAt(position);

    endRemoveRows();
    return true;
}

std::expected<const HistoryCacheData*, QString> HistoryModel::getHistoryCacheData(const int inIdx) const
{
    return solCore->historyManager()->getHistoryCacheData(inIdx);
}


void HistoryModel::updateHistoryCache()
{
    beginResetModel();

    endResetModel();
}
