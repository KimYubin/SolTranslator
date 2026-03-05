// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMANAGER_H
#define SOLTRANSLATOR_HISTORYMANAGER_H


#include <QThread>

#include <expected>

#include "AbstractManager.h"
#include "SolHashQueue.h"
#include "SolTypes.h"

#include "Widgets/HistoryCacheData.h"

class HistoryCacheData;
class SolTranslatorCore;
class QSqlError;
class QTimer;

using LookupResult = std::tuple<bool, QString>;

class HistoryManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit HistoryManager(SolTranslatorCore* parent);
    virtual ~HistoryManager() override;

    void asyncAddHistory(const EngineType inEngineType
                       , const LangType inSourceLang
                       , const LangType inTargetLang
                       , const QString& inOriginText
                       , const QString& inTranslateText
                       , const TextStyle inTextStyle);

    void asyncDeleteHistory(const qint64 inDbId);

    /**
     * 번역 기록이 있다면, 번역문을 반환합니다.
     * 해당 번역의 최근 기록을 추가합니다.
     * 
     * @return first - 번역이 있다면 true. second - 번역문
     */
    void asyncLookupHistory(const EngineType inEngineType
                          , const QString& inOriginText
                          , const LangType inSourceLang
                          , const LangType inTargetLang
                          , QObject* inContext
                          , std::move_only_function<void(const LookupResult&)> inFinishedFunction);

private: signals :
    void sigLookupHistory(const EngineType inEngineType
                          , const QString& inOriginText
                          , const LangType inSourceLang
                          , const LangType inTargetLang
                          , QObject* inContext);
    void sigAddHistory(const EngineType inEngineType
                     , const LangType inSourceLang
                     , const LangType inTargetLang
                     , const QString& inOriginText
                     , const QString& inTranslateText
                     , const TextStyle inTextStyle);
    void sigDeleteHistory(const qint64 inDbId);
    void sigChangeTranslateHistory();

public slots :
    void lookupFinished(const LookupResult& inLookup, QObject* inContext);
    void historyUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

    std::expected<const HistoryCacheData*, QString> getTranslateCache(const int inIdx);
    int getTranslateCacheSize() const { return _translateTextCache.size(); };

    bool setCheckState(const int inIdx, const Qt::CheckState inState);

    int findModelIdxFromTimelineId(const qint64 inTimelineId, const QDateTime& inTimeStamp);


private:
    std::vector<HistoryCacheData> _translateTextCache;

    QThread m_workerThread;

    std::unordered_map<QObject*, std::move_only_function<void(const LookupResult&)>> _requestCallbacks;
};


#endif //SOLTRANSLATOR_HISTORYMANAGER_H
