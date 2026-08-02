// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryModel.h"

#include "SolTranslatorCore.h"
#include "Managers/HistoryManager.h"
#include "Utils/SolLog.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/HistoryCacheData.h"

#include <QRegularExpression>

HistoryModel::HistoryModel(QObject* inParent)
    : QAbstractListModel(inParent)
{
    connect(solCore->manager<HistoryManager>(), &HistoryManager::translateHistoryUpdated, this, &HistoryModel::updateHistoryCache);
}

int HistoryModel::rowCount(const QModelIndex& inParent) const
{
    return inParent.isValid() ? 0 : solCore->manager<HistoryManager>()->getHistoryCacheSize();
}

int HistoryModel::columnCount(const QModelIndex& inParent) const
{
    return inParent.isValid() ? 0 : 1;
}

QVariant HistoryModel::data(const QModelIndex& inIndex, const int inRole) const
{
    const Expected<const HistoryCacheData*> trCache = getHistoryCacheData(inIndex.row());

    if (!trCache)
    {
        solDebug << trCache.error() << "- role :" << inRole;
        return QVariant();
    }

    switch (inRole)
    {
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

bool HistoryModel::setData(const QModelIndex& inIndex, const QVariant& inValue, const int inRole)
{
    if (inIndex.isValid() == false)
    {
        return false;
    }

    switch (inRole)
    {
    case Sol::CheckRole:
    {
        solCore->manager<HistoryManager>()->setCheckState(inIndex.row(), static_cast<Qt::CheckState>(inValue.toInt()));
        return true;
    }
    default:
        break;
    }

    return false;
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex& inIndex) const
{
    if (inIndex.isValid() == false)
    {
        return Qt::ItemIsEnabled;
    }

    return QAbstractListModel::flags(inIndex)
            | Qt::ItemIsEnabled
            | Qt::ItemIsUserCheckable;
}

bool HistoryModel::insertRows(const int inPosition, const int inRows, const QModelIndex& inIndex)
{
    Q_UNUSED(inIndex);
    beginInsertRows(QModelIndex(), inPosition, inPosition + inRows - 1);

    // for (int row = 0; row < rows; ++row)
    //     _historyList.insert(position, {QString(), QString()});

    endInsertRows();
    return true;
}

bool HistoryModel::removeRows(const int inPosition, const int inRows, const QModelIndex& inIndex)
{
    Q_UNUSED(inIndex);
    beginRemoveRows(QModelIndex(), inPosition, inPosition + inRows - 1);

    // for (int row = 0; row < rows; ++row)
        // _historyList.removeAt(position);

    endRemoveRows();
    return true;
}

Expected<const HistoryCacheData*> HistoryModel::getHistoryCacheData(const int inIdx) const
{
    return solCore->manager<HistoryManager>()->getHistoryCacheData(inIdx);
}

Expected<const HistoryCacheData*> HistoryModel::getHistoryCacheData(const QModelIndex& inIndex) const
{
    return getHistoryCacheData(inIndex.row());
}


void HistoryModel::updateHistoryCache()
{
    beginResetModel();

    endResetModel();
}
