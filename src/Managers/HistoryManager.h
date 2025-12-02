// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYMANAGER_H
#define SOLTRANSLATOR_HISTORYMANAGER_H


#include <deque>

#include "AbstractManager.h"
#include "SolHashQueue.h"
#include "SolTypes.h"

class SolTranslatorCore;
class QSqlError;
class QTimer;

class HistoryManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit HistoryManager(SolTranslatorCore* parent);
    virtual ~HistoryManager() override;

private:
    QSqlError initializeDB();

public:
    void addHistory(const EngineType inEngineType
                  , const LangType inSourceLang
                  , const LangType inTargetLang
                  , const QString& inOriginText
                  , const QString& inTranslateText
                  , const TextStyle inTextStyle);

    /**
     * 번역 기록이 있다면, 번역문을 반환합니다.
     * 해당 번역의 최근 기록을 추가합니다.
     * 
     * @return first - 번역이 있다면 true. second - 번역문
     */
    std::tuple<bool, QString> lookupHistory(const EngineType inEngineType
                                          , const QString& inOriginText
                                          , const LangType inSourceLang
                                          , const LangType inTargetLang);

    void markDbDirty();

signals:
    void translateHistoryChanged(const std::deque<trDbInfo>& inHistoryList);

private:
    void applyTranslateHistory();

    // 연속으로 너무 빨리 업데이트 되는 것을 방지하기 위한 타이머.
    // emit translateHistoryChanged
    QTimer* _dbUpdateTimer;

    bool _bIsDirtyDB = true;
};


#endif //SOLTRANSLATOR_HISTORYMANAGER_H
