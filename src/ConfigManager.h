//
// Created by YubinKim on 25/03/10 월.
//

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QObject>
#include <QSettings>

#include "FinTypes.h"


class ConfigManager : public QObject
{
    Q_OBJECT

private:
    ConfigManager();

public:
    static ConfigManager& get()
    {
        static ConfigManager* instance = new ConfigManager();
        return *instance;
    }

    void setCurrentEngineType(EngineType inEngineType);
    EngineType getCurrentEngineType();

    void setAPIKey(EngineType inEngineType, const QString& inAPIKey);
    QString getAPIKey(EngineType inEngineType);

    void setOpenAIModel(const QString& inModelName);
    QString getOpenAIModel();

private:
    QSettings* _settings;
};


#endif //CONFIGMANAGER_H
