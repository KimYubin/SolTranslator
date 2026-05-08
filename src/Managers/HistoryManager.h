// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMANAGER_H
#define SOLTRANSLATOR_HISTORYMANAGER_H

#include "AbstractManager.h"
#include "Types/SolExpected.hpp"
#include "Widgets/HistoryCacheData.h"

#include <QThread>

class EngineId;
enum class TextStyle;
enum class LangType;
class HistoryCacheData;
class SolTranslatorCore;


class HistoryManager : public AbstractManager
{
    Q_OBJECT
    using LookupResult = std::tuple<bool, QString>;

public:
    explicit HistoryManager(SolTranslatorCore* parent);
    virtual ~HistoryManager() override;

    void asyncAddHistory(const EngineId& inEngineId
                       , const LangType inSourceLang
                       , const LangType inTargetLang
                       , const QString& inSourceText
                       , const QString& inTargetText
                       , const TextStyle inTextStyle);

    void asyncDeleteHistory(const qint64 inDbId);

    /**
     * 번역 기록찾고, 찾았다면 최근 기록을 갱신합니다.
     */
    void asyncLookupHistory(const EngineId& inEngineId
                          , const QString& inSourceText
                          , const LangType inSourceLang
                          , const LangType inTargetLang
                          , QObject* inContext
                          , std::move_only_function<void(const LookupResult&)> inFinishedFunction);

signals:
    void requestHistoryLookup(const EngineId& inEngineId
                            , const QString& inSourceText
                            , const LangType inSourceLang
                            , const LangType inTargetLang
                            , const int inReqId);
    void requestAddHistory(const EngineId& inEngineId
                         , const LangType inSourceLang
                         , const LangType inTargetLang
                         , const QString& inSourceText
                         , const QString& inTargetText
                         , const TextStyle inTextStyle);
    void requestDeleteHistory(const qint64 inDbId);
    void translateHistoryUpdated();

public slots :
    void onLookupFinished(const LookupResult& inLookup, const int inReqId);
    void onDbCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

    Expected<const HistoryCacheData*> getHistoryCacheData(const int inIdx) const;
    int getHistoryCacheSize() const { return _historyCaches.size(); }

    bool setCheckState(const int inIdx, const Qt::CheckState inState);

    Expected<int> findModelIdxFromTimelineId(const qint64 inTimelineId, const QDateTime& inTimeStamp) const;

private:
    std::vector<HistoryCacheData> _historyCaches;

    QThread _workerThread;

    struct reqCallback
    {
        QPointer<QObject> context;
        std::move_only_function<void(const LookupResult&)> callback;
    };
    std::unordered_map<int, reqCallback> _requestCallbacks;
};


#endif //SOLTRANSLATOR_HISTORYMANAGER_H
