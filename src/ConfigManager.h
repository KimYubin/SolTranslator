//
// Created by YubinKim on 25/03/10 월.
//

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QObject>
#include <QSettings>


class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager& get()
    {
        static ConfigManager* instance = new ConfigManager();
        return *instance;
    }

    void setAPI(const QString& inAPI);

    QString getAPI();

private:
    QSettings GetAPISetting();

    ConfigManager() = default;
};


#endif //CONFIGMANAGER_H
