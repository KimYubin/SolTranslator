// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTimer>

#include "DbWorker.h"
#include "SolDatabase.h"
#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolUtilibrary.h"

namespace
{
const char* db_type = "QSQLITE";
const char* db_connectionName = "sol_db";
} // anonymous namespace

HistoryManager::HistoryManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    DbWorker* worker = new DbWorker();
    connect(&m_workerThread, &QThread::started, worker, &DbWorker::initializeDB);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    worker->moveToThread(&m_workerThread);

    connect(this, &HistoryManager::sigLookupHistory, worker, &DbWorker::lookupHistory);
    connect(worker, &DbWorker::sigFinishLookup, this, &HistoryManager::lookupFinished);

    connect(this, &HistoryManager::sigAddHistory, worker, &DbWorker::addHistory);
    connect(this, &HistoryManager::sigDeleteHistory, worker, &DbWorker::deleteHistory);
    connect(worker, &DbWorker::sigUpdateHistoryCache, this, &HistoryManager::historyUpdated);

    m_workerThread.start();
}

HistoryManager::~HistoryManager()
{
    QSqlDatabase historyDB = QSqlDatabase::database();
    historyDB.close();
}

void HistoryManager::asyncAddHistory(const EngineType inEngineType
                                   , const LangType inSourceLang
                                   , const LangType inTargetLang
                                   , const QString& inOriginText
                                   , const QString& inTranslateText
                                   , const TextStyle inTextStyle)
{
    emit sigAddHistory(inEngineType
                     , inSourceLang
                     , inTargetLang
                     , inOriginText
                     , inTranslateText
                     , inTextStyle);
}

void HistoryManager::asyncDeleteHistory(const qint64 inDbId)
{
    emit sigDeleteHistory(inDbId);
}

void HistoryManager::asyncLookupHistory(const EngineType inEngineType
                                      , const QString& inOriginText
                                      , const LangType inSourceLang
                                      , const LangType inTargetLang
                                      , QObject* inContext
                                      , std::move_only_function<void(const LookupResult&)> inFinishedFunction)
{
    _requestCallbacks[inContext] = std::move(inFinishedFunction);
    connect(inContext, &QObject::destroyed, this, [this, inContext]() { _requestCallbacks.erase(inContext); });

    emit sigLookupHistory(inEngineType, inOriginText, inSourceLang, inTargetLang, inContext);
}

void HistoryManager::lookupFinished(const LookupResult& inLookup, QObject* inContext)
{
    const auto it = _requestCallbacks.
            find(inContext);
    if (it == _requestCallbacks.end())
    {
        return;
    }
    it->second(inLookup);
    _requestCallbacks.erase(it);
}

void HistoryManager::historyUpdated(const std::vector<HistoryCacheData>& inCacheDatas)
{
    _translateTextCache = inCacheDatas;

    emit sigChangeTranslateHistory();
}

std::expected<const HistoryCacheData*, QString> HistoryManager::getTranslateCache(const int inIdx)
{
    if (inIdx < 0 || inIdx >= _translateTextCache.size())
    {
        return std::unexpected("_translateTextCache out of range :"
            "\n - size: " + QString::number(_translateTextCache.size())
            + "\n - inIdx: " + QString::number(inIdx));
    }

    return &_translateTextCache[inIdx];
}

bool HistoryManager::setCheckState(const int inIdx, const Qt::CheckState inState)
{
    if (inIdx < 0 || inIdx >= _translateTextCache.size())
    {
        return false;
    }

    _translateTextCache[inIdx].setCheckState(inState);
    return true;
}

int HistoryManager::findModelIdxFromTimelineId(const qint64 inTimelineId
                                             , const QDateTime& inTimeStamp)
{
    const auto lowIt = std::ranges::lower_bound(_translateTextCache, inTimeStamp, std::greater<QDateTime>(), &HistoryCacheData::getTimeStamp);
    if (lowIt == _translateTextCache.end() || lowIt->getTimeStamp() != inTimeStamp)
    {
        return -1;
    }
    const auto upperIt = std::ranges::upper_bound(lowIt, _translateTextCache.end(), inTimeStamp, std::greater<QDateTime>(), &HistoryCacheData::getTimeStamp);

    const std::vector<HistoryCacheData>::iterator findIt = std::find_if(lowIt, upperIt, [inTimelineId](const HistoryCacheData& inCache)
    {
        return inCache.getTimelineId() == inTimelineId;
    });

    if (findIt == _translateTextCache.end())
    {
        return -1;
    }

    return findIt - _translateTextCache.begin();
}
