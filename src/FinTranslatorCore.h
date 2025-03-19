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
    FinTranslatorCore(QObject* parent = nullptr);
    ~FinTranslatorCore();

public:
    void onSimpleTranslate(const QString& InOriginText);

    DataManager* getDataManager() const { return dataManager; }
    TranslateManager* getTranslateManager() const { return translateManager; }
    GlobalHotKeyManager* getGlobalHotKeyManager() const { return globalHotKeyManager; }

private:
    FinTranslatorMainWidget* finMainWidget;

    DataManager* dataManager;
    TranslateManager* translateManager;
    GlobalHotKeyManager* globalHotKeyManager;
};


#endif //FINTRANSLATORCORE_H
