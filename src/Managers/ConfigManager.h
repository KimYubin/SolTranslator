//
// Created by YubinKim on 25/03/10 월.
//

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H
#include <QObject>
#include <QSettings>

#include "FinTypes.h"
#include "magic_enum.hpp"


class ConfigManager : public QObject
{
    Q_OBJECT

private:
    ConfigManager();

public:
    static ConfigManager& get()
    {
        static ConfigManager* instance = new ConfigManager();
        return *instance;
    }

    void setCurrentEngineType(EngineType inEngineType);
    EngineType getCurrentEngineType();

    void setAPIKey(EngineType inEngineType, const QString& inAPIKey);
    QString getAPIKey(EngineType inEngineType);

    void setOpenAIModel(const QString& inModelName);
    QString getOpenAIModel();

    void setStartRun(const bool inStartRun);
    bool getStartRun();

    void setSimplePopupGeometry(const QRect& inGeo);
    QRect getSimplePopupGeometry();
    void setSimplePopupScreenPolicy(const Fin::ScreenPopupPolicy& inPolicy);
    Fin::ScreenPopupPolicy getSimplePopupScreenPolicy();

private:
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
    const std::string_view enumMemberName = magic_enum::enum_name<EnumType>(inVal);
    _settings->setValue(inKey, enumMemberName.data());
}

template <typename EnumType>
        requires std::is_enum_v<EnumType>
EnumType ConfigManager::getEnumValue(const QAnyStringView& inKey, const EnumType inDefaultVal)
{
    const QVariant variant = _settings->value(inKey, magic_enum::enum_name<EnumType>(inDefaultVal).data());
    const QString str      = variant.toString();
    EnumType policy        = magic_enum::enum_cast<EnumType>(str.toStdString()).value_or(inDefaultVal);
    return policy;
}


#endif //CONFIGMANAGER_H
