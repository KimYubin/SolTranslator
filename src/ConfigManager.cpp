//
// Created by YubinKim on 25/03/10 월.
//

#include "ConfigManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

#include "FinConstants.h"
#include "FinTypes.h"

const QString Engine_Type = "Engine_Type";
const QString API_Key     = "API_Key/";

ConfigManager::ConfigManager()
{
    _settings = new QSettings(FinPaths::getApiKeyPath(), QSettings::IniFormat, this);
}


void ConfigManager::setCurrentEngineType(EngineType inEngineType)
{
    _settings->setValue(Engine_Type, EnumToInt(inEngineType));
}

EngineType ConfigManager::getCurrentEngineType()
{
    return static_cast<EngineType>(_settings->value(Engine_Type, EnumToInt(EngineType::OpenAI)).toInt());
}


void ConfigManager::setAPIKey(EngineType inEngineType, const QString& inAPIKey)
{
    _settings->setValue(API_Key + EngineName::getName(inEngineType), inAPIKey);
    _settings->sync();
}

QString ConfigManager::getAPIKey(EngineType inEngineType)
{
    return _settings->value(API_Key + EngineName::getName(inEngineType)).toString();
}


void ConfigManager::setOpenAIModel(const QString& inModelName)
{
    _settings->setValue("openai_model", inModelName);
}

QString ConfigManager::getOpenAIModel()
{
    // gpt-4o-mini    // gpt-4.1-mini
    return _settings->value("openai_model", "gpt-4o-mini").toString();
}

void ConfigManager::setStartRun(const bool inStartRun)
{
    _settings->setValue(Fin::Const::CommandLineOptions::START_UP_RUN, inStartRun);

    const QString appName = QCoreApplication::applicationName();
    const QString appPath = QCoreApplication::applicationFilePath();
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
                     , QSettings::NativeFormat);
    
    if (inStartRun)
    {
        settings.setValue(appName, QDir::toNativeSeparators(appPath) + " --" + Fin::Const::CommandLineOptions::START_UP_RUN.data());

        QSettings approvedSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartupApproved\\Run"
                                 , QSettings::NativeFormat);
        QByteArray enabledData;
        enabledData.resize(8);
        enabledData[0] = 0x02; // Enabled
        for (int i = 1; i < 8; ++i)
        {
            enabledData[i] = 0x00;
        }
        approvedSettings.setValue(appName, enabledData);
    }
    else
    {
        settings.remove(appName);
    }
}

bool ConfigManager::getStartRun()
{
    return _settings->value(Fin::Const::CommandLineOptions::START_UP_RUN, false).toBool();
}
