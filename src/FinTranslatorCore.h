//
// Created by YubinKim on 25/03/19 수.
//

#ifndef FINTRANSLATORCORE_H
#define FINTRANSLATORCORE_H
#include <QObject>


class FinTranslatorMainWidget;
class GlobalHotKeyManager;
class TranslateManager;
class DataManager;

class FinTranslatorCore : public QObject
{
    Q_OBJECT

public:
    explicit FinTranslatorCore(QObject* parent = nullptr);
    ~FinTranslatorCore() override;

public:
    void onSimpleTranslate(const QString& InOriginText);

    FinTranslatorMainWidget* getFinMainWidget() { return _finMainWidget; }

    DataManager* getDataManager() const { return _dataManager; }
    TranslateManager* getTranslateManager() const { return _translateManager; }
    GlobalHotKeyManager* getGlobalHotKeyManager() const { return _globalHotKeyManager; }

private:
    FinTranslatorMainWidget* _finMainWidget;

    DataManager* _dataManager;
    TranslateManager* _translateManager;
    GlobalHotKeyManager* _globalHotKeyManager;
};


#endif //FINTRANSLATORCORE_H
