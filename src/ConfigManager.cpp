//
// Created by YubinKim on 25/03/10 월.
//

#include "ConfigManager.h"

#include <QSettings>

#include "FinTypes.h"

void ConfigManager::setAPI(const QString& inAPI)
{
    GetAPISetting().setValue(EngineName::OPEN_AI, inAPI);
}

QString ConfigManager::getAPI()
{
    return GetAPISetting().value(EngineName::OPEN_AI).toString();
}

QSettings ConfigManager::GetAPISetting()
{
    return QSettings{FinPaths::getApiKeyPath(), QSettings::IniFormat};
}
