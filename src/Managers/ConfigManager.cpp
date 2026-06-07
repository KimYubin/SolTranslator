// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ConfigManager.h"

#include "EngineManager.h"
#include "SecretStore.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "qtkeychain/keychain.h"
#include "Types/OptionKey.h"
#include "Types/SolConstants.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolLog.h"
#include "Utils/SolPath.h"

#include <QCoreApplication>
#include <QDir>
#include <QRectF>
#include <QSettings>
#include <QWidget>

#include <magic_enum.hpp>

namespace
{
const QString CurrentEngine = "CurrentEngine";

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
void setEnumValue(QSettings* inSettings, const QString& inKey, const EnumType inVal)
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
EnumType enumValue(const QSettings* inSettings, const QString& inKey, const EnumType inDefault)
{
    const QString defaultQStr = Sol::enumToQStr(inDefault);

    const QString setting_value_str = inSettings->value(inKey, defaultQStr).toString();

    EnumType policy = magic_enum::enum_cast<EnumType>(setting_value_str.toStdString()).value_or(inDefault);

    return policy;
}

} // anonymous namespace

ConfigManager::ConfigManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    _settings    = new QSettings(SolPath::absolute(SolFile::Config), QSettings::IniFormat, this);
    _secretStore = new SecretStore(this);
}


void ConfigManager::loadSecretKey(const QString& inKey
                                , LoadCallback&& inCallback)
{
    _secretStore->requestLoadSecret(inKey, std::move(inCallback));
}

void ConfigManager::loadSecretKey(const EngineId& inEngineId
                                , const OptionKey& inOptionKey
                                , LoadCallback&& inCallback)
{
    loadSecretKey(Sol::engineOptionKey(inEngineId, inOptionKey), std::move(inCallback));
}

void ConfigManager::loadSecretKeys(const std::vector<QString>& inKeyList
                                 , Callback<void()>&& inCallback)
{
    _secretStore->requestLoadSecretList(inKeyList, std::move(inCallback));
}


void ConfigManager::setSecretKey(const QString& inKey
                               , const QVariant& inValue
                               , Callback<void()>&& inCallback)
{
    _secretStore->requestSaveSecret(inKey, inValue, std::move(inCallback));
}

QVariant ConfigManager::secretKey(const QString& inKey, const QVariant& inDefault) const
{
    return _secretStore->getSecret(inKey, inDefault.toString());;
}

Expected<void> ConfigManager::setEngineOption(const EngineId& inEngineId
                                            , const OptionKey& inKey
                                            , const QVariant& inValue)
{
    const ITranslateEngine* trEngine = EngineManager::getEngine(inEngineId).get();
    const Expected<const OptionSpec*> optExp = trEngine->getOptionSpec(inKey);
    if (!optExp)
    {
        return makeUnexpected(QString{
            "Not found Engine OptionSpec. "
            "Check the registration of the engine OptionSpec. "
            "Engine: %1, key: %2"
        }.arg(inEngineId.toString(), inKey.toString()));
    }

    const OptionSpec& optSpec = *optExp.value();
    const QString egOptKey    = Sol::engineOptionKey(inEngineId, inKey);

    if (optSpec.isSecretMode)
    {
        setSecretKey(egOptKey, inValue);
        return {};
    }

    _settings->setValue(egOptKey, inValue);
    return {};
}

Expected<QVariant> ConfigManager::engineOption(const EngineId& inEngineId
                                             , const OptionKey& inKey) const
{
    const ITranslateEngine* trEngine = EngineManager::getEngine(inEngineId).get();
    const Expected<const OptionSpec*> optExp = trEngine->getOptionSpec(inKey);
    if (!optExp)
    {
        return makeUnexpected(QString{
            "Not found Engine OptionSpec. "
            "Check the registration of the engine OptionSpec. "
            "Engine: %1, key: %2"
        }.arg(inEngineId.toString(), inKey.toString()));
    }

    const OptionSpec& optSpec = *optExp.value();
    const QString egOptKey    = Sol::engineOptionKey(inEngineId, inKey);
    const QVariant defaultVal = optSpec.getDefaultValue();

    if (optSpec.isSecretMode)
    {
        return secretKey(egOptKey, defaultVal);
    }

    return _settings->value(egOptKey, defaultVal);
}

void ConfigManager::setCurrentEngineId(const EngineId& inEngineId)
{
    _settings->setValue(CurrentEngine, inEngineId.toString());
}

EngineId ConfigManager::currentEngineId() const
{
    return EngineId{_settings->value(CurrentEngine, EngineIds::defaultEngine.toString()).toString()};
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
    if (isRememberWindowGeometry())
    {
        setSaveGeometry(inWidget->objectName() + WidgetGeometry, inWidget->saveGeometry());
    }
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
const std::unordered_map<Action, QKeySequence> defaultShortcuts
{
    {Action::None,                  QKeySequence{}}

  , {Action::MainWidgetRaise,       QKeySequence{Qt::ALT | Qt::Key_V}}
  , {Action::PopupTranslate,        QKeySequence{Qt::ALT | Qt::Key_C}}

  , {Action::SettingsClose,         QKeySequence{Qt::CTRL | Qt::Key_W}}
  , {Action::SettingsOpen,          QKeySequence{Qt::Key_F5}}
  , {Action::MainClose,             QKeySequence{Qt::CTRL | Qt::Key_W}}
  , {Action::MainNextTab,           QKeySequence{Qt::CTRL | Qt::Key_Tab}}
  , {Action::MainPrevTab,           QKeySequence{Qt::CTRL | Qt::SHIFT | Qt::Key_Tab}}
  , {Action::TextTab,               QKeySequence{Qt::CTRL | Qt::Key_T}}
  , {Action::DocumentTab,           QKeySequence{Qt::CTRL | Qt::Key_D}}
  , {Action::HistoryTab,            QKeySequence{Qt::CTRL | Qt::Key_H}}
  , {Action::EngineSelector,        QKeySequence{Qt::CTRL | Qt::Key_E}}

  , {Action::SourceTargetToggle,    QKeySequence{Qt::Key_T}}
  , {Action::CopyDoc,               QKeySequence{Qt::Key_C}}
  , {Action::ViewInPopup,           QKeySequence{Qt::Key_V}}
  , {Action::ReTranslate,           QKeySequence{Qt::CTRL | Qt::Key_R}}
  , {Action::DeleteTranslation,     QKeySequence{/*Qt::CTRL | Qt::SHIFT | Qt::Key_D*/}}

  , {Action::PopupAlwaysOn,         QKeySequence{Qt::Key_A}}
  , {Action::PopupWindowMode,       QKeySequence{Qt::Key_N}}
  , {Action::PopupMinimize,         QKeySequence{Qt::Key_M}}
  , {Action::PopupMaxRestore,       QKeySequence{Qt::Key_G}}
  , {Action::PopupClose,            QKeySequence{Qt::Key_Escape}}

};

constexpr int ActionCheck = 22;
static_assert(static_cast<int>(Action::Size) == ActionCheck, "Action changed: update defaultShortcuts");

} // anonymous namespace


void ConfigManager::setShortCut(const Action inShortCut, const QKeySequence& inKeySequence)
{
    _settings->setValue(Shortcuts + Sol::enumToQStr(inShortCut), inKeySequence);
}

QKeySequence ConfigManager::shortcut(const Action inShortCut) const
{
    QKeySequence defaultKey{};
    const auto findit = defaultShortcuts.find(inShortCut);
    if (findit != defaultShortcuts.end())
    {
        defaultKey = findit->second;
    }

    return _settings->value(Shortcuts + Sol::enumToQStr(inShortCut), defaultKey).value<QKeySequence>();
}

void ConfigManager::setSaveGeometry(const QString& inKey, const QByteArray& inGeoData)
{
    _settings->setValue(inKey, inGeoData);
}

std::tuple<bool, QByteArray> ConfigManager::saveGeometry(const QString& inKey) const
{
    const QVariant valVariant = _settings->value(inKey);

    return {valVariant.isValid(), valVariant.toByteArray()};
}
