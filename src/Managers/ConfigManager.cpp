// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "ConfigManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QRectF>
#include <QSettings>
#include <QWidget>

#include "../../external/magic_enum.hpp"

#include "SolConstants.h"
#include "SolTypes.h"

namespace
{
const QString Engine_Type = "Engine_Type";
const QString API_Key     = "API_Key/";

const QString OpenAI_Model = "openai_model";
const QString OpenAI_Temperature = "openai_temperature";

const QString PopupTargetLanguageType = "PopupTargetLanguageType";

const QString TextSrcLangType    = "TextEditSourceLanguageType";
const QString TextTargetLangType = "TextEditTargetLanguageType";

const QString SimplePopupGeometry     = "SimplePopupGeometry";
const QString SimplePopupScreenPolicy = "SimplePopupScreenPolicy";

const QString IsRememberWindowGeo = "IsRememberWindowGeo";
const QString WidgetGeometry      = "WidgetGeometry";

const QString IsPopupTrWindowTemp = "IsPopupTrWindowTemp";

const QString UserGuide        = "UserGuide/";
const QString FirstCloseToTray = "FirstCloseToTray";

const QString TimeFormat    = "TimeFormat/";
const QString HistoryFormat = "HistoryFormat";

const QString Shortcuts = "Shortcuts/";
} // anonymous namespace

ConfigManager::ConfigManager()
{
    _settings = new QSettings(SolPaths::getConfigPath(), QSettings::IniFormat, this);
}


void ConfigManager::setCurrentEngineType(EngineType inEngineType)
{
    setEnumValue(Engine_Type, inEngineType);
}

EngineType ConfigManager::getCurrentEngineType()
{
    return getEnumValue(Engine_Type, EngineHelper::getDefaultEngineType());
}


void ConfigManager::setAPIKey(EngineType inEngineType, const QString& inAPIKey)
{
    _settings->setValue(API_Key + EngineHelper::getName(inEngineType), inAPIKey);
    _settings->sync();
}

QString ConfigManager::getAPIKey(EngineType inEngineType)
{
    return _settings->value(API_Key + EngineHelper::getName(inEngineType)).toString();
}


void ConfigManager::setOpenAIModel(const QString& inModelName)
{
    _settings->setValue(OpenAI_Model, inModelName);
}

QString ConfigManager::getOpenAIModel()
{
    // gpt-4o-mini / gpt-4.1-mini
    return _settings->value(OpenAI_Model, "gpt-4o-mini").toString();
}

double ConfigManager::defaultAI_Temperature() const
{
    return 0.5;
}

void ConfigManager::setOpenAI_Temperature(const double inTemperature)
{
    _settings->setValue(OpenAI_Temperature, inTemperature);
}

double ConfigManager::getOpenAI_Temperature()
{
    return _settings->value(OpenAI_Temperature, defaultAI_Temperature()).toDouble();
}

void ConfigManager::setStartRun(const bool inStartRun)
{
    _settings->setValue(Sol::CmdLineOptions::START_UP_RUN, inStartRun);

    const QString appName = QCoreApplication::applicationName();
    const QString appPath = QCoreApplication::applicationFilePath();
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
                     , QSettings::NativeFormat);

    if (inStartRun)
    {
        settings.setValue(appName, QDir::toNativeSeparators(appPath) + " --" + Sol::CmdLineOptions::START_UP_RUN);

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
    return _settings->value(Sol::CmdLineOptions::START_UP_RUN, false).toBool();
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

void ConfigManager::setSimplePopupScreenPolicy(const ScreenPopupPolicy& inPolicy)
{
    setEnumValue(SimplePopupScreenPolicy, inPolicy);
}

ScreenPopupPolicy ConfigManager::getSimplePopupScreenPolicy()
{
    return getEnumValue(SimplePopupScreenPolicy, ScreenPopupPolicy::CursorScreen);
}

void ConfigManager::setIsRememberWindowGeometry(const bool inIsRememberWindowGeometry)
{
    _settings->setValue(IsRememberWindowGeo, inIsRememberWindowGeometry);
}

bool ConfigManager::getIsRememberWindowGeometry()
{
    return _settings->value(IsRememberWindowGeo, false).toBool();
}

void ConfigManager::saveWidgetGeometry(const QWidget* inWidget)
{
    setSaveGeometry(inWidget->objectName() + WidgetGeometry, inWidget->saveGeometry());
}

bool ConfigManager::restoreWidgetGeometry(QWidget* inWidget)
{
    if (getIsRememberWindowGeometry() == false)
    {
        return false;
    }

    const auto [bIsExistGeo, geoByteArr] = getSaveGeometry(inWidget->objectName() + WidgetGeometry);

    if (bIsExistGeo)
    {
        return inWidget->restoreGeometry(geoByteArr);
    }
    return false;
}

void ConfigManager::setIsPopupTrWindowTemp(const bool inTemp)
{
    _settings->setValue(IsPopupTrWindowTemp, inTemp);
}

bool ConfigManager::getIsPopupTrWindowTemp()
{
    return _settings->value(IsPopupTrWindowTemp, false).toBool();
}


void ConfigManager::setFirstCloseToTray()
{
    _settings->setValue(UserGuide + FirstCloseToTray, false);
}

bool ConfigManager::isFirstCloseToTray()
{
    return _settings->value(UserGuide + FirstCloseToTray, true).toBool();
}

void ConfigManager::setHistoryTimeFormat(const QString& inFormat)
{
    _settings->setValue(TimeFormat + HistoryFormat, inFormat);
}

QString ConfigManager::getHistoryTimeFormat()
{
    return _settings->value(TimeFormat + HistoryFormat, "yyyy/MM/dd (ddd) hh:mm").toString();
}

namespace
{
const std::unordered_map<Action, QKeySequence> defaultShortcut
{
    {Action::None,            QKeySequence()}

  , {Action::PopupTranslate,  QKeySequence(Qt::ALT | Qt::Key_C)}

  , {Action::SettingsClose,   QKeySequence(Qt::CTRL | Qt::Key_W)}
  , {Action::SettingsOpen,    QKeySequence(Qt::Key_F5)}
  , {Action::MainClose,       QKeySequence(Qt::CTRL | Qt::Key_W)}
  , {Action::MainNextTab,     QKeySequence(Qt::CTRL | Qt::Key_Tab)}
  , {Action::MainPrevTab,     QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab)}

  , {Action::HistoryToggle,   QKeySequence(Qt::Key_T)}
  , {Action::DocCopyButton,   QKeySequence(Qt::Key_C)}

  , {Action::PopupAlwaysOn,   QKeySequence(Qt::Key_A)}
  , {Action::PopupWindowMode, QKeySequence(Qt::Key_N)}
  , {Action::PopupMinimize,   QKeySequence(Qt::Key_M)}
  , {Action::PopupMaxRestore, QKeySequence(Qt::Key_G)}
  , {Action::PopupClose,      QKeySequence(Qt::Key_Escape)}
  , {Action::PopupToggle,     QKeySequence(Qt::Key_T)}

};

} // anonymous namespace

void ConfigManager::setShortCut(const Action inShortCut, const QKeySequence& inKeySequence)
{
    _settings->setValue(Shortcuts + Sol::enumToQStr(inShortCut), inKeySequence);
}

QKeySequence ConfigManager::shortcut(const Action inShortCut) const
{
    return _settings->value(Shortcuts + Sol::enumToQStr(inShortCut), defaultShortcut.at(inShortCut)).value<QKeySequence>();
}

void ConfigManager::setSaveGeometry(const QAnyStringView& inKey, const QByteArray& inGeoData) const
{
    _settings->setValue(inKey, inGeoData);
}

std::tuple<bool, QByteArray> ConfigManager::getSaveGeometry(const QAnyStringView& inKey) const
{
    const QVariant valVariant = _settings->value(inKey);

    return {valVariant.isValid(), valVariant.toByteArray()};
}
