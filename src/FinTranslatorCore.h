// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATORCORE_H
#define FINTRANSLATORCORE_H
#include <QObject>
#include <QApplication>

#include "FinTypes.h"


class AsyncManager;
class QMimeData;
class FinTranslatorMainWidget;
class GlobalHotKeyManager;
class TranslateManager;
class DataManager;

#define finCore FinTranslatorCore::instance()

/**
 * FinTranslator의 Non-UI 관련 기능과 mainWidget을 관리하는 최상위 객체입니다.
 * 객체는 전역에서 유일하게 존재해야 합니다.
 */
class FinTranslatorCore : public QObject
{
    Q_OBJECT

public:
    explicit FinTranslatorCore(QObject* parent = nullptr);
    ~FinTranslatorCore() override;

    static FinTranslatorCore* instance() noexcept { return _self; }

public:
    void loadCache();
    void asyncSaveCache();
    void onSimpleTranslate(const QMimeData* inMimeData);

    DataManager* getDataManager() const { return _dataManager; }
    TranslateManager* getTranslateManager() const { return _translateManager; }
    GlobalHotKeyManager* getGlobalHotKeyManager() const { return _globalHotKeyManager; }
    AsyncManager* getAsyncManager() const { return _asyncManager; };

    FinTranslatorMainWidget* getFinMainWidget() const { return _finMainWidget; }

private:
    static FinTranslatorCore* _self;

    DataManager* _dataManager;
    TranslateManager* _translateManager;
    GlobalHotKeyManager* _globalHotKeyManager;
    AsyncManager* _asyncManager;

    FinTranslatorMainWidget* _finMainWidget;
};


#endif //FINTRANSLATORCORE_H
