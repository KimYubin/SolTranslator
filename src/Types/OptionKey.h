// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_OPTIONKEY_H
#define SOLTRANSLATOR_OPTIONKEY_H

#include <QHashFunctions>
#include <QString>

#include <algorithm>
#include <compare>


class OptionKey
{
public:
    OptionKey() = default;

    explicit OptionKey(const QString& inStr) : _value(inStr) {}
    explicit OptionKey(QString&& inStr) noexcept : _value(std::move(inStr)) {}
    OptionKey(const OptionKey& inOther) = default;
    OptionKey(OptionKey&& inOther) noexcept : _value(std::move(inOther._value)) {}

    OptionKey& operator=(const OptionKey& inOther)
    {
        if (this == &inOther)
            return *this;
        _value = inOther._value;
        return *this;
    }

    OptionKey& operator=(OptionKey&& inOther) noexcept
    {
        if (this == &inOther)
            return *this;
        _value = std::move(inOther._value);
        return *this;
    }

    std::strong_ordering operator<=>(const OptionKey& other) const = default;
    bool operator==(const OptionKey& other) const = default;


    const QString& toString() const { return _value; }

private:
    QString _value;
};

struct OptionKey_hasher
{
    size_t operator()(const OptionKey& inKy) const
    {
        return std::hash<QString>()(inKy.toString());
    }
};

#endif //SOLTRANSLATOR_OPTIONKEY_H
