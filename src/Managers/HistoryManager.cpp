// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "HistoryManager.h"

#include <QDateTime>

#include "DbWorker.h"
#include "SolTranslatorCore.h"

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
{}

void HistoryManager::asyncAddHistory(const EngineType inEngineType
                                   , const LangType inSourceLang
                                   , const LangType inTargetLang
                                   , const QString& inOriginText
                                   , const QString& inTranslateText
                                   , const TextStyle inTextStyle)
{
    emit requestAddHistory(inEngineType
                         , inSourceLang
                         , inTargetLang
                         , inOriginText
                         , inTranslateText
                         , inTextStyle);
}

void HistoryManager::asyncDeleteHistory(const qint64 inDbId)
{
    emit requestDeleteHistory(inDbId);
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

    emit requestHistoryLookup(inEngineType, inOriginText, inSourceLang, inTargetLang, inContext);
}

void HistoryManager::onLookupFinished(const LookupResult& inLookup, QObject* inContext)
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

void HistoryManager::onDbCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas)
{
    _translateTextCache = inCacheDatas;

    emit translateHistoryUpdated();
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
