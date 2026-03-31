// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_DBWORKER_H
#define SOLTRANSLATOR_DBWORKER_H
#include <QObject>


enum class LangType;
enum class TextStyle;
enum class EngineType;
class HistoryCacheData;
class QTimer;

class DbWorker : public QObject
{
    Q_OBJECT

public:
    explicit DbWorker(QObject* parent = nullptr);
    virtual ~DbWorker() override;

    void initialize();

private:
    void initDB();

public
slots :
    void processAddHistory(const EngineType inEngineType
                         , const LangType inSourceLang
                         , const LangType inTargetLang
                         , const QString& inSourceText
                         , const QString& inTargetText
                         , const TextStyle inTextStyle);

    void processDeleteHistory(const qint64 inDbId);

    /**
     * Lookup translation history, and if found, update the most recent history.
     */
    void processLookupHistory(const EngineType inEngineType
                            , const QString& inSourceText
                            , const LangType inSourceLang
                            , const LangType inTargetLang
                            , const int inReqId);

private:
    std::tuple<bool, QString> lookupHistoryImpl(const EngineType inEngineType
                                              , const QString& inSourceText
                                              , const LangType inSourceLang
                                              , const LangType inTargetLang);

public:
signals:
    void lookupFinished(const std::tuple<bool, QString>& inLookup
                      , const int inReqId);
    void historyCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

private:
    void updateDbCache();

    void markDbDirty();

    /**
     * Run wal_checkpoint. If processed with TRUNCATE, it may take more time.
     * @param inIsTRUNCATE If true, processed with TRUNCATE; otherwise with PASSIVE.
     */
    void runCheckpoint(const bool inIsTRUNCATE = false);

    /**
     * 연속으로 너무 빨리 업데이트 되는 것을 방지하기 위한 타이머.
     * emit historyCacheUpdated
     */
    QTimer* _dbUpdateTimer;

    bool _isDirtyDB = true;
};

#endif //SOLTRANSLATOR_DBWORKER_H
