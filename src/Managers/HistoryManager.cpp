// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include "DbWorker.h"
#include "Utils/SolLog.h"

#include <QDateTime>

HistoryManager::HistoryManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    DbWorker* dbWorker = new DbWorker();
    connect(&_workerThread, &QThread::started, dbWorker, &DbWorker::initialize);
    connect(&_workerThread, &QThread::finished, dbWorker, &QObject::deleteLater);
    dbWorker->moveToThread(&_workerThread);

    connect(this, &HistoryManager::requestHistoryLookup, dbWorker, &DbWorker::processLookupHistory);
    connect(dbWorker, &DbWorker::lookupFinished, this, &HistoryManager::onLookupFinished);

    connect(this, &HistoryManager::requestAddHistory, dbWorker, &DbWorker::processAddHistory);
    connect(this, &HistoryManager::requestDeleteHistory, dbWorker, &DbWorker::processDeleteHistory);
    connect(dbWorker, &DbWorker::historyCacheUpdated, this, &HistoryManager::onDbCacheUpdated);

    _workerThread.start();
}

HistoryManager::~HistoryManager()
{
    _workerThread.requestInterruption();
    _workerThread.quit();
    _workerThread.wait();
}

void HistoryManager::asyncAddHistory(const EngineType inEngineType
                                   , const LangType inSourceLang
                                   , const LangType inTargetLang
                                   , const QString& inSourceText
                                   , const QString& inTargetText
                                   , const TextStyle inTextStyle)
{
    emit requestAddHistory(inEngineType
                         , inSourceLang
                         , inTargetLang
                         , inSourceText
                         , inTargetText
                         , inTextStyle);
}

void HistoryManager::asyncDeleteHistory(const qint64 inDbId)
{
    emit requestDeleteHistory(inDbId);
}

namespace
{
qint64 newRequestId()
{
    static qint64 requestID{0};
    requestID++;
    return requestID;
}
}
void HistoryManager::asyncLookupHistory(const EngineType inEngineType
                                      , const QString& inSourceText
                                      , const LangType inSourceLang
                                      , const LangType inTargetLang
                                      , QObject* inContext
                                      , std::move_only_function<void(const LookupResult&)> inFinishedFunction)
{
    const int requestID = newRequestId();

    _requestCallbacks[requestID] = {inContext, (std::move(inFinishedFunction))};
    connect(inContext, &QObject::destroyed, this, [this, requestID]() { _requestCallbacks.erase(requestID); });

    emit requestHistoryLookup(inEngineType, inSourceText, inSourceLang, inTargetLang, requestID);
}

void HistoryManager::onLookupFinished(const LookupResult& inLookup, const int inReqId)
{
    const auto it = _requestCallbacks.find(inReqId);
    if (it == _requestCallbacks.end())
    {
        return;
    }

    if (it->second.context)
    {
        it->second.callback(inLookup);
    }

    _requestCallbacks.erase(it);
}

void HistoryManager::onDbCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas)
{
    _historyCaches = inCacheDatas;

    emit translateHistoryUpdated();
}

std::expected<const HistoryCacheData*, QString> HistoryManager::getHistoryCacheData(const int inIdx) const
{
    if (inIdx < 0 || inIdx >= _historyCaches.size())
    {
        return std::unexpected("_historyCaches out of range :"
            "\n - size: " + QString::number(_historyCaches.size())
            + "\n - inIdx: " + QString::number(inIdx));
    }

    return &_historyCaches[inIdx];
}

bool HistoryManager::setCheckState(const int inIdx, const Qt::CheckState inState)
{
    if (inIdx < 0 || inIdx >= _historyCaches.size())
    {
        return false;
    }

    _historyCaches[inIdx].setCheckState(inState);
    return true;
}

std::expected<int, QString> HistoryManager::findModelIdxFromTimelineId(const qint64 inTimelineId
                                                                     , const QDateTime& inTimeStamp) const
{
    const auto lowIt = std::ranges::lower_bound(_historyCaches, inTimeStamp, std::greater<QDateTime>(), &HistoryCacheData::getTimeStamp);
    if (lowIt == _historyCaches.end() || lowIt->getTimeStamp() != inTimeStamp)
    {
        return std::unexpected{"not found TimeStamp. TimeStamp: " + inTimeStamp.toString()};
    }
    const auto upperIt = std::ranges::upper_bound(lowIt, _historyCaches.end(), inTimeStamp, std::greater<QDateTime>(), &HistoryCacheData::getTimeStamp);

    const auto findIt = std::find_if(lowIt, upperIt, [inTimelineId](const HistoryCacheData& inCache)
    {
        return inCache.getTimelineId() == inTimelineId;
    });

    if (findIt == _historyCaches.end())
    {
        return std::unexpected{QString{"not found Timeline ID. TimeStamp: %1, ID: %2"}.arg(inTimeStamp.toString(), inTimelineId)};
    }

    return findIt - _historyCaches.begin();
}
