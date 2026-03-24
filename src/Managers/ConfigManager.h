// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QSettings>

#include "AbstractManager.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include "../../external/magic_enum.hpp"

#if defined(solConfig)
#undef solConfig
#endif
#define solConfig (*solCore->configManager())

/**
 *The ConfigManager class stores and manages configuration/settings.
 */
class ConfigManager: public AbstractManager
{
    Q_OBJECT

public:
    explicit ConfigManager(SolTranslatorCore* parent);

    void setCurrentEngineType(const EngineType inEngineType);
    EngineType currentEngineType() const;

    void setApiKey(const EngineType inEngineType, const QString& inAPIKey);
    QString apiKey(const EngineType inEngineType) const;

    void setOpenAIModel(const QString& inModelName);
    QString openAIModel() const;

    double defaultAI_Temperature() const;
    void setOpenAI_Temperature(const double inTemperature);
    double openAI_Temperature() const;

    void setStartRun(const bool inStartRun);
    bool startRun() const;

    /** 팝업 번역 도착 언어 */
    void setPopupTargetLang(const LangType inLangType);
    LangType popupTargetLang() const;

    /** TextEdit 번역 출발 언어 */
    void setTextSrcLang(const LangType inLangType);
    LangType textSrcLang() const;

    /** TextEdit 번역 도착 언어 */
    void setTextTargetLang(const LangType inLangType);
    LangType textTargetLang() const;

    void setSimplePopupGeometry(const QRect& inGeo);
    QRect simplePopupGeometry() const;
    void setSimplePopupScreenPolicy(const ScreenPopupPolicy& inPolicy);
    ScreenPopupPolicy simplePopupScreenPolicy() const;

    /** 창의 위치와 크기를 기억 유무를 저장합니다. */
    void setIsRememberWindowGeometry(const bool inIsRememberWindowGeometry);
    bool isRememberWindowGeometry() const;

    /** 창의 위치와 크기를 저장합니다. */
    void saveWidgetGeometry(const QWidget* inWidget);
    bool restoreWidgetGeometry(QWidget* inWidget) const;

    /**
     * 팝업 번역창의 임시창 유무.
     * 임시창이면 포커스 이탈시, 창이 자동으로 닫힙니다.
     * 기본값은 false입니다.
     */
    void setIsPopupTrWindowTemp(const bool inTemp);
    bool isPopupTrWindowTemp() const;

    /** 메인 창을 처음 닫은 후 호출합니다.*/
    void setFirstCloseToTray();

    /** 메인 창을 처음 닫았나요? 그렇다면, 앱이 트레이로 숨겨졌음을 안내해야 합니다. */
    bool isFirstCloseToTray() const;

    void setHistoryTimeFormat(const QString& inFormat);
    QString historyTimeFormat() const;

    // ~======================
    // shortcut
    void setShortCut(const Action inShortCut, const QKeySequence& inKeySequence);
    QKeySequence shortcut(const Action inShortCut) const;

private:
    void setSaveGeometry(const QAnyStringView& inKey, const QByteArray& inGeoData);
    std::tuple<bool, QByteArray> saveGeometry(const QAnyStringView& inKey) const;

    /**
     * enum type 설정을 문자열로 저장합니다.
     * 
     * @tparam EnumType enum, enum class
     * @param inKey 저장에 사용할 key
     * @param inVal 저장할 enum 
     */
    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    void setEnumValue(const QAnyStringView& inKey, const EnumType inVal);

    /**
     * 문자열로 저장된 enum type 설정을 불러옵니다.
     * EnumType과 정확히 동일한 문자열이 아니면 기본값을 반환합니다.
     * 
     * @tparam EnumType 
     * @param inKey 설정 key
     * @param inDefault 저장값이 없는 경우와 유효하지 않은 경우 반환할 값
     * @return 
     */
    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    EnumType enumValue(const QAnyStringView& inKey, const EnumType inDefault) const;

private:
    QSettings* _settings;
};


template <typename EnumType>
    requires std::is_enum_v<EnumType>
void ConfigManager::setEnumValue(const QAnyStringView& inKey, const EnumType inVal)
{
    _settings->setValue(inKey, Sol::enumToQStr(inVal));
}

template <typename EnumType>
    requires std::is_enum_v<EnumType>
EnumType ConfigManager::enumValue(const QAnyStringView& inKey, const EnumType inDefault) const
{
    const QString defaultQStr = Sol::enumToQStr(inDefault);

    const QString setting_value_str = _settings->value(inKey, defaultQStr).toString();

    EnumType policy = magic_enum::enum_cast<EnumType>(setting_value_str.toStdString()).value_or(inDefault);

    return policy;
}


#endif //CONFIGMANAGER_H
