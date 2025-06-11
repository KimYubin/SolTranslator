//
// Created by YubinKim on 25/03/19 수.
//

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
