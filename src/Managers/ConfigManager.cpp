//
// Created by YubinKim on 25/03/10 월.
//

#include "ConfigManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QRectF>
#include <QSettings>

#include "magic_enum.hpp"

#include "FinConstants.h"
#include "FinTypes.h"


const QString Engine_Type = "Engine_Type";
const QString API_Key     = "API_Key/";

const QString PopupTargetLanguageType = "PopupTargetLanguageType";

const QString TextSrcLangType    = "TextEditSourceLanguageType";
const QString TextTargetLangType = "TextEditTargetLanguageType";

const QString SimplePopupGeometry     = "SimplePopupGeometry";
const QString SimplePopupScreenPolicy = "SimplePopupScreenPolicy";

ConfigManager::ConfigManager()
{
    _settings = new QSettings(FinPaths::getApiKeyPath(), QSettings::IniFormat, this);
}


void ConfigManager::setCurrentEngineType(EngineType inEngineType)
{
    setEnumValue(Engine_Type, inEngineType);
}

EngineType ConfigManager::getCurrentEngineType()
{
    return getEnumValue(Engine_Type, EngineType::FinPoint);
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

void ConfigManager::setPopupTargetLang(const LangType inLangType)
{
    setEnumValue(PopupTargetLanguageType, inLangType);
}

LangType ConfigManager::getPopupTargetLang()
{
    return getEnumValue(PopupTargetLanguageType, LangType::ko);
}

void ConfigManager::setTextSrcLang(const LangType inLangType)
{
    setEnumValue(TextSrcLangType, inLangType);
}

LangType ConfigManager::getTextSrcLang()
{
    return getEnumValue(TextSrcLangType, LangType::AUTO);
}

void ConfigManager::setTextTargetLang(const LangType inLangType)
{
    setEnumValue(TextTargetLangType, inLangType);
}

LangType ConfigManager::getTextTargetLang()
{
    return getEnumValue(TextTargetLangType, LangType::ko);
}

void ConfigManager::setSimplePopupGeometry(const QRect& inGeo)
{
    _settings->setValue(SimplePopupGeometry, inGeo);
}

QRect ConfigManager::getSimplePopupGeometry()
{
    return _settings->value(SimplePopupGeometry).toRect();
}

void ConfigManager::setSimplePopupScreenPolicy(const Fin::ScreenPopupPolicy& inPolicy)
{
    setEnumValue(SimplePopupScreenPolicy, inPolicy);
}

Fin::ScreenPopupPolicy ConfigManager::getSimplePopupScreenPolicy()
{
    return getEnumValue(SimplePopupScreenPolicy, Fin::ScreenPopupPolicy::CursorScreen);
}
