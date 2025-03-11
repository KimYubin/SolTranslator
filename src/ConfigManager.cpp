//
// Created by YubinKim on 25/03/10 월.
//

#include "ConfigManager.h"
#include <QSettings>

void ConfigManager::setAPI(const QString& inAPI)
{
    QSettings settings("api", QSettings::IniFormat);

    settings.setValue("openai", inAPI);
}

QString ConfigManager::getAPI()
{
    QSettings settings("api", QSettings::IniFormat);
    return settings.value("openai").toString();
}
