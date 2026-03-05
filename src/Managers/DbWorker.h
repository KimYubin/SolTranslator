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

    void initializeDB();

public
slots :
    void addHistory(const EngineType inEngineType
                  , const LangType inSourceLang
                  , const LangType inTargetLang
                  , const QString& inOriginText
                  , const QString& inTranslateText
                  , const TextStyle inTextStyle);

    void deleteHistory(const qint64 inDbId);

private:
    std::tuple<bool, QString> lookupHistoryImpl(const EngineType inEngineType
                                              , const QString& inOriginText
                                              , const LangType inSourceLang
                                              , const LangType inTargetLang);
    /**
     * 번역 기록이 있다면, 번역문을 반환합니다.
     * 해당 번역의 최근 기록을 추가합니다.
     * 
     * @return first - 번역이 있다면 true. second - 번역문
     */
public:
    void lookupHistory(const EngineType inEngineType
                     , const QString& inOriginText
                     , const LangType inSourceLang
                     , const LangType inTargetLang
                     , QObject* inContext);

public:
signals :
    void lookupFinished(const std::tuple<bool, QString>& inLookup
                      , QObject* inContext);
    void sigHistoryUpdated(const std::vector<HistoryCacheData>& inCacheDatas);

private:
    void updateDbCache();

    void markDbDirty();

    // 연속으로 너무 빨리 업데이트 되는 것을 방지하기 위한 타이머.
    // emit sigHistoryUpdated
    QTimer* _dbUpdateTimer;

    bool _bIsDirtyDB = true;

};

#endif //SOLTRANSLATOR_DBWORKER_H
