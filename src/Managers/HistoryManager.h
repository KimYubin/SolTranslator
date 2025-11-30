// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYMANAGER_H
#define FINTRANSLATOR_HISTORYMANAGER_H


#include <deque>

#include "AbstractManager.h"
#include "FinHashQueue.h"
#include "FinTypes.h"

class FinTranslatorCore;
class QSqlError;

class HistoryManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit HistoryManager(FinTranslatorCore* parent);
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

    int getHistoryCount();

    const std::deque<trDbInfo> getTranslateTextCache();

    void markDbDirty();

signals:
    void translateHistoryChanged();

private:
    /** 캐시된 번역문.*/
    // std::deque<trDbInfo> _translateTextCache;

    bool _bIsDirtyDB = true;
};


#endif //FINTRANSLATOR_HISTORYMANAGER_H
