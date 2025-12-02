// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATORCORE_H
#define SOLTRANSLATORCORE_H
#include <QObject>
#include <QApplication>

#include "SolTypes.h"


class HistoryManager;
class AsyncManager;
class QMimeData;
class SolMainWidget;
class GlobalHotKeyManager;
class TranslateManager;
class DataManager;

#define solCore SolTranslatorCore::instance()

/**
 * SolTranslator의 Non-UI 관련 기능과 mainWidget을 관리하는 최상위 객체입니다.
 * 객체는 전역에서 유일하게 존재해야 합니다.
 */
class SolTranslatorCore : public QObject
{
    Q_OBJECT

public:
    explicit SolTranslatorCore(QObject* parent = nullptr);
    ~SolTranslatorCore() override;

    static SolTranslatorCore* instance() noexcept { return _self; }

public:
    void onSimpleTranslate(const QMimeData* inMimeData);

    DataManager* dataManager() const { return _dataManager; }
    TranslateManager* translateManager() const { return _translateManager; }
    HistoryManager* historyManager() const { return _historyManager; }
    GlobalHotKeyManager* globalHotKeyManager() const { return _globalHotKeyManager; }
    AsyncManager* asyncManager() const { return _asyncManager; };

    SolMainWidget* solMainWidget() const { return _solMainWidget; }

private:
    static SolTranslatorCore* _self;

    DataManager* _dataManager;
    TranslateManager* _translateManager;
    HistoryManager* _historyManager;
    GlobalHotKeyManager* _globalHotKeyManager;
    AsyncManager* _asyncManager;

    SolMainWidget* _solMainWidget;
};


#endif //SOLTRANSLATORCORE_H
