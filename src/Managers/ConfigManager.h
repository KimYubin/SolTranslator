// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QObject>
#include <QSettings>

#include "FinTypes.h"
#include "FinUtilibrary.h"

#include "../../external/magic_enum.hpp"


#define finConfig ConfigManager::instance()

/**
 * 프로그램의 구성, 설정 등의 상태 정보를 저장 및 관리합니다.
 * 영구 저장이 필요한 관리 데이터는 이곳에서 저장합니다.
 * 전역에서 접근할 수 있는 싱글톤 객체로 사용합니다.
 */
class ConfigManager : public QObject
{
    Q_OBJECT

private:
    ConfigManager();

public:
    static ConfigManager& instance()
    {
        static ConfigManager* configInstance = new ConfigManager();
        return *configInstance;
    }

    void setCurrentEngineType(EngineType inEngineType);
    EngineType getCurrentEngineType();

    void setAPIKey(EngineType inEngineType, const QString& inAPIKey);
    QString getAPIKey(EngineType inEngineType);

    void setOpenAIModel(const QString& inModelName);
    QString getOpenAIModel();

    void setStartRun(const bool inStartRun);
    bool getStartRun();

    /** 팝업 번역 도착 언어 */
    void setPopupTargetLang(const LangType inLangType);
    LangType getPopupTargetLang();

    /** TextEdit 번역 출발 언어 */
    void setTextSrcLang(const LangType inLangType);
    LangType getTextSrcLang();

    /** TextEdit 번역 도착 언어 */
    void setTextTargetLang(const LangType inLangType);
    LangType getTextTargetLang();

    void setSimplePopupGeometry(const QRect& inGeo);
    QRect getSimplePopupGeometry();
    void setSimplePopupScreenPolicy(const Fin::ScreenPopupPolicy& inPolicy);
    Fin::ScreenPopupPolicy getSimplePopupScreenPolicy();

    /** 창의 위치와 크기를 기억 유무를 저장합니다. */
    void setIsRememberWindowGeometry(const bool inIsRememberWindowGeometry);
    bool getIsRememberWindowGeometry();

    /** 창의 위치와 크기를 저장합니다. */
    void saveWidgetGeometry(const QWidget* inWidget);
    bool restoreWidgetGeometry(QWidget* inWidget);

    /** 메인 창을 처음 닫은 후 호출합니다.*/
    void setFirstCloseToTray();
    /** 메인 창을 처음 닫았나요? 트레이 아이콘으로 숨겨졌음을 안내해야 합니다. */
    bool isFirstCloseToTray();
private:
    void setSaveGeometry(const QAnyStringView& inKey, const QByteArray& inGeoData) const;
    std::tuple<bool, QByteArray> getSaveGeometry(const QAnyStringView& inKey) const;

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
     * @param inDefaultVal 저장값이 없는 경우와 유효하지 않은 경우 반환할 값
     * @return 
     */
    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    EnumType getEnumValue(const QAnyStringView& inKey, const EnumType inDefaultVal);

private:
    QSettings* _settings;
};


template <typename EnumType>
    requires std::is_enum_v<EnumType>
void ConfigManager::setEnumValue(const QAnyStringView& inKey, const EnumType inVal)
{
    _settings->setValue(inKey, Fin::enumToQStr(inVal));
}

template <typename EnumType>
        requires std::is_enum_v<EnumType>
EnumType ConfigManager::getEnumValue(const QAnyStringView& inKey, const EnumType inDefaultVal)
{
    const QString defaultQStr = Fin::enumToQStr(inDefaultVal);

    const QString setting_value_str = _settings->value(inKey, defaultQStr).toString();

    EnumType policy = magic_enum::enum_cast<EnumType>(setting_value_str.toStdString()).value_or(inDefaultVal);

    return policy;
}


#endif //CONFIGMANAGER_H
