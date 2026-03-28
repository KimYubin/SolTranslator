// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMANAGER_H
#define SOLTRANSLATOR_HISTORYMANAGER_H

#include "AbstractManager.h"
#include "Widgets/HistoryCacheData.h"

#include <QThread>

#include <expected>

enum class TextStyle;
enum class LangType;
enum class EngineType;
class HistoryCacheData;
class SolTranslatorCore;


class HistoryManager : public AbstractManager
{
    Q_OBJECT
    using LookupResult = std::tuple<bool, QString>;

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
     * 번역 기록찾고, 찾았다면 최근 기록을 갱신합니다.
     */
    void asyncLookupHistory(const EngineType inEngineType
                          , const QString& inOriginText
                          , const LangType inSourceLang
                          , const LangType inTargetLang
                          , QObject* inContext
                          , std::move_only_function<void(const LookupResult&)> inFinishedFunction);

signals:
    void requestHistoryLookup(const EngineType inEngineType
                            , const QString& inOriginText
                            , const LangType inSourceLang
                            , const LangType inTargetLang
                            , QObject* inContext);
    void requestAddHistory(const EngineType inEngineType
                         , const LangType inSourceLang
                         , const LangType inTargetLang
                         , const QString& inOriginText
                         , const QString& inTranslateText
                         , const TextStyle inTextStyle);
    void requestDeleteHistory(const qint64 inDbId);
    void translateHistoryUpdated();

public slots :
    void onLookupFinished(const LookupResult& inLookup, QObject* inContext);
    void onDbCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

    std::expected<const HistoryCacheData*, QString> getTranslateCache(const int inIdx) const;
    int getTranslateCacheSize() const { return _translateTextCache.size(); };

    bool setCheckState(const int inIdx, const Qt::CheckState inState);

    std::expected<int, QString> findModelIdxFromTimelineId(const qint64 inTimelineId, const QDateTime& inTimeStamp) const;

private:
    std::vector<HistoryCacheData> _translateTextCache;

    QThread _workerThread;

    std::unordered_map<QObject*, std::move_only_function<void(const LookupResult&)>> _requestCallbacks;
};


#endif //SOLTRANSLATOR_HISTORYMANAGER_H
