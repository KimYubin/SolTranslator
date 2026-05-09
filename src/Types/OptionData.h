// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_OPTIONDATA_H
#define SOLTRANSLATOR_OPTIONDATA_H

#include "Types/OptionKey.h"
#include "Utils/EnumUtils.hpp"

#include <QString>
#include <QStringList>

template <typename T>
struct _BaseSpinData
{
    _BaseSpinData() = default;

    _BaseSpinData(const T& inMin
                , const T& inMax
                , const T& inVal
                , const T& inSingleStep)
        : min(inMin)
        , max(inMax)
        , value(inVal)
        , singleStep(inSingleStep)
    {}

    T min;
    T max;
    T value;
    T singleStep;
};

template <typename T>
struct SpinData : public _BaseSpinData<T>
{
    using _BaseSpinData<T>::_BaseSpinData;
};

template <std::floating_point T>
struct SpinData<T> : public _BaseSpinData<T>
{
    SpinData() = default;

    explicit SpinData(const T& inMin
                    , const T& inMax
                    , const T& inVal
                    , const T& inSingleStep
                    , const int inDecimals)
        : _BaseSpinData<T>(inMin, inMax, inVal, inSingleStep)
        , decimals(inDecimals) {}

    int decimals;
};

struct ComboList
{
    QString defaultString;
    QStringList list;
};

struct StringSaver
{
    StringSaver(QString inString
              , const bool inIsUsedSaveButton = false)
        : defaultString(std::move(inString))
        , isUsedSaveButton(inIsUsedSaveButton)
    {}

    StringSaver()                                          = default;
    ~StringSaver()                                         = default;
    StringSaver(const StringSaver& inOther)                = default;
    StringSaver(StringSaver&& inOther) noexcept            = default;
    StringSaver& operator=(const StringSaver& inOther)     = default;
    StringSaver& operator=(StringSaver&& inOther) noexcept = default;

    QString defaultString;
    bool isUsedSaveButton = false;
};

using OptionVariant = std::variant<std::monostate, int, double, bool, SpinData<int>, SpinData<double>, StringSaver, ComboList>;


struct OptionData
{
    OptionData(OptionKey inKey
             , QString inHeaderName
             , std::optional<QString> inDescription
             , OptionVariant inDefaultValue
             , const bool inIsSecretMode = false)
        : key(std::move(inKey))
        , headerName(std::move(inHeaderName))
        , description(std::move(inDescription))
        , defaultValue(std::move(inDefaultValue))
        , isSecretMode(inIsSecretMode)
    {}

    OptionData()                                         = default;
    ~OptionData()                                        = default;
    OptionData(const OptionData& inOther)                = default;
    OptionData(OptionData&& inOther) noexcept            = default;
    OptionData& operator=(const OptionData& inOther)     = default;
    OptionData& operator=(OptionData&& inOther) noexcept = default;

    enum class Type
    {
        None, Int, Double, Bool, SpinDataInt, SpinDataDouble, StringSaver, Combo
    };

    Type getOptionDataType() const
    {
        return static_cast<Type>(defaultValue.index());
    };

    QVariant getDefaultValue() const;


    // ~===============
    // Variables

    OptionKey key;

    QString headerName;
    std::optional<QString> description;
    OptionVariant defaultValue;
    bool isSecretMode = false;

    mutable int insertionOrder = std::numeric_limits<int>::max();
};


#endif //SOLTRANSLATOR_OPTIONDATA_H
