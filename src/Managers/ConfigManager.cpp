// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ConfigManager.h"

#include "SolConstants.h"
#include "SolPath.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include "../../external/magic_enum.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QRectF>
#include <QSettings>
#include <QWidget>

namespace
{
const QString Engine_Type = "Engine_Type";
const QString API_Key     = "API_Key/";

const QString OpenAI_Model = "openai_model";
const QString OpenAI_Temperature = "openai_temperature";

const QString PopupTargetLanguage = "PopupTargetLanguage";

const QString TextSrcLangType    = "TextEditSourceLanguageType";
const QString TextTargetLangType = "TextEditTargetLanguageType";

const QString PopupGeometry     = "PopupGeometry";
const QString PopupScreenPolicy = "PopupScreenPolicy";

const QString IsRememberWindowGeo = "IsRememberWindowGeo";
const QString WidgetGeometry      = "WidgetGeometry";

const QString IsPopupTrWindowTemp = "IsPopupTrWindowTemp";

const QString UserGuide        = "UserGuide/";
const QString FirstCloseToTray = "FirstCloseToTray";

const QString TimeFormat    = "TimeFormat/";
const QString HistoryFormat = "HistoryFormat";

const QString Shortcuts = "Shortcuts/";


/**
 * enum type 설정을 문자열로 저장합니다.
 * 
 * @tparam EnumType enum, enum class
 * @param inSettings QSettings 멤버
 * @param inKey 저장에 사용할 key
 * @param inVal 저장할 enum 
 */
template <typename EnumType>
    requires std::is_enum_v<EnumType>
void setEnumValue(QSettings* inSettings, const QAnyStringView& inKey, const EnumType inVal)
{
    inSettings->setValue(inKey, Sol::enumToQStr(inVal));
}

/**
 * 문자열로 저장된 enum type 설정을 불러옵니다.
 * EnumType과 정확히 동일한 문자열이 아니면 기본값을 반환합니다.
 * 
 * @tparam EnumType 
 * @param inSettings QSettings 멤버
 * @param inKey 설정 key
 * @param inDefault 저장값이 없는 경우와 유효하지 않은 경우 반환할 값
 * @return 
 */
template <typename EnumType>
    requires std::is_enum_v<EnumType>
EnumType enumValue(const QSettings* inSettings, const QAnyStringView& inKey, const EnumType inDefault)
{
    const QString defaultQStr = Sol::enumToQStr(inDefault);

    const QString setting_value_str = inSettings->value(inKey, defaultQStr).toString();

    EnumType policy = magic_enum::enum_cast<EnumType>(setting_value_str.toStdString()).value_or(inDefault);

    return policy;
}


} // anonymous namespace

ConfigManager::ConfigManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    _settings = new QSettings(SolPath::absolute(SolFile::Config), QSettings::IniFormat, this);
}


void ConfigManager::setCurrentEngineType(const EngineType inEngineType)
{
    setEnumValue(_settings, Engine_Type, inEngineType);
}

EngineType ConfigManager::currentEngineType() const
{
    return enumValue(_settings, Engine_Type, EngineHelper::defaultEngineType());
}


void ConfigManager::setApiKey(const EngineType inEngineType, const QString& inAPIKey)
{
    _settings->setValue(API_Key + Sol::enumToQStr(inEngineType), inAPIKey);
}

QString ConfigManager::apiKey(const EngineType inEngineType) const
{
    return _settings->value(API_Key + Sol::enumToQStr(inEngineType)).toString();
}


void ConfigManager::setOpenAIModel(const QString& inModelName)
{
    _settings->setValue(OpenAI_Model, inModelName);
}

QString ConfigManager::openAIModel() const
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

double ConfigManager::openAI_Temperature() const
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

bool ConfigManager::startRun() const
{
    return _settings->value(Sol::CmdLineOptions::START_UP_RUN, false).toBool();
}

void ConfigManager::setPopupTargetLang(const LangType inLangType)
{
    setEnumValue(_settings, PopupTargetLanguage, inLangType);
}

LangType ConfigManager::popupTargetLang() const
{
    return enumValue(_settings, PopupTargetLanguage, LangType::ko);
}

void ConfigManager::setTextSrcLang(const LangType inLangType)
{
    setEnumValue(_settings, TextSrcLangType, inLangType);
}

LangType ConfigManager::textSrcLang() const
{
    return enumValue(_settings, TextSrcLangType, LangType::AUTO);
}

void ConfigManager::setTextTargetLang(const LangType inLangType)
{
    setEnumValue(_settings, TextTargetLangType, inLangType);
}

LangType ConfigManager::textTargetLang() const
{
    return enumValue(_settings, TextTargetLangType, LangType::ko);
}

void ConfigManager::setPopupGeometry(const QRect& inGeo)
{
    _settings->setValue(PopupGeometry, inGeo);
}

QRect ConfigManager::popupGeometry() const
{
    return _settings->value(PopupGeometry).toRect();
}

void ConfigManager::setPopupScreenPolicy(const ScreenPopupPolicy& inPolicy)
{
    setEnumValue(_settings, PopupScreenPolicy, inPolicy);
}

ScreenPopupPolicy ConfigManager::popupScreenPolicy() const
{
    return enumValue(_settings, PopupScreenPolicy, ScreenPopupPolicy::CursorScreen);
}

void ConfigManager::setIsRememberWindowGeometry(const bool inIsRememberWindowGeometry)
{
    _settings->setValue(IsRememberWindowGeo, inIsRememberWindowGeometry);
}

bool ConfigManager::isRememberWindowGeometry() const
{
    return _settings->value(IsRememberWindowGeo, false).toBool();
}

void ConfigManager::saveWidgetGeometry(const QWidget* inWidget)
{
    setSaveGeometry(inWidget->objectName() + WidgetGeometry, inWidget->saveGeometry());
}

bool ConfigManager::restoreWidgetGeometry(QWidget* inWidget) const
{
    if (isRememberWindowGeometry() == false)
    {
        return false;
    }

    const auto [bIsExistGeo, geoByteArr] = saveGeometry(inWidget->objectName() + WidgetGeometry);

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

bool ConfigManager::isPopupTrWindowTemp() const
{
    return _settings->value(IsPopupTrWindowTemp, false).toBool();
}


void ConfigManager::setFirstCloseToTray()
{
    _settings->setValue(UserGuide + FirstCloseToTray, false);
}

bool ConfigManager::isFirstCloseToTray() const
{
    return _settings->value(UserGuide + FirstCloseToTray, true).toBool();
}

void ConfigManager::setHistoryTimeFormat(const QString& inFormat)
{
    _settings->setValue(TimeFormat + HistoryFormat, inFormat);
}

QString ConfigManager::historyTimeFormat() const
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

void ConfigManager::setSaveGeometry(const QAnyStringView& inKey, const QByteArray& inGeoData)
{
    _settings->setValue(inKey, inGeoData);
}

std::tuple<bool, QByteArray> ConfigManager::saveGeometry(const QAnyStringView& inKey) const
{
    const QVariant valVariant = _settings->value(inKey);

    return {valVariant.isValid(), valVariant.toByteArray()};
}
