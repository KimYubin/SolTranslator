// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENGINEID_H
#define SOLTRANSLATOR_ENGINEID_H

#include <QHashFunctions>
#include <QString>

#include <algorithm>
#include <compare>


class EngineId
{
public:
    EngineId() = default;

    explicit EngineId(const QString& inStr) : _value(inStr) {}
    explicit EngineId(QString&& inStr) noexcept : _value(std::move(inStr)) {}
    EngineId(const EngineId& inOther) : _value(inOther._value) {}
    EngineId(EngineId&& inOther) noexcept : _value(std::move(inOther._value)) {}

    EngineId& operator=(const EngineId& inOther)
    {
        if (this == &inOther)
            return *this;
        _value = inOther._value;
        return *this;
    }

    EngineId& operator=(EngineId&& inOther) noexcept
    {
        if (this == &inOther)
            return *this;
        _value = std::move(inOther._value);
        return *this;
    }

    std::strong_ordering operator<=>(const EngineId& other) const = default;
    bool operator==(const EngineId& other) const = default;


    const QString& toString() const { return _value; }

private:
    QString _value;
};

struct EngineId_hasher
{
    size_t operator()(const EngineId& inKy) const
    {
        return std::hash<::QString>()(inKy.toString());
    }
};


#endif //SOLTRANSLATOR_ENGINEID_H
