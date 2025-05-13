//
// Created by YubinKim on 25/03/10 월.
//

#include "ConfigManager.h"

#include <QSettings>

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
    return _settings->value("openai_model", "gpt-4.1-mini").toString();
}
