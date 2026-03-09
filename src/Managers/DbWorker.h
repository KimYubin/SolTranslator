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
                         , const QString& inOriginText
                         , const QString& inTranslateText
                         , const TextStyle inTextStyle);

    void processDeleteHistory(const qint64 inDbId);

    /**
     * 번역 기록찾고, 찾았다면 최근 기록을 갱신합니다.
     */
    void processLookupHistory(const EngineType inEngineType
                            , const QString& inOriginText
                            , const LangType inSourceLang
                            , const LangType inTargetLang
                            , QObject* inContext);

private:
    std::tuple<bool, QString> lookupHistoryImpl(const EngineType inEngineType
                                              , const QString& inOriginText
                                              , const LangType inSourceLang
                                              , const LangType inTargetLang);

public:
signals:
    void lookupFinished(const std::tuple<bool, QString>& inLookup
                      , QObject* inContext);
    void historyCacheUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

private:
    void updateDbCache();

    void markDbDirty();

    void runCheckpoint(const bool inIsTRUNCATE = false);
    /**
     * 연속으로 너무 빨리 업데이트 되는 것을 방지하기 위한 타이머.
     * emit historyCacheUpdated
     */
    QTimer* _dbUpdateTimer;

    bool _bIsDirtyDB = true;
};

#endif //SOLTRANSLATOR_DBWORKER_H
