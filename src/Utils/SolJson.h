// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLJSONOBJECT_H
#define SOLTRANSLATOR_SOLJSONOBJECT_H
#include <QJsonObject>
#include <QJsonValue>

#include <expected>


class SolJsonValueRef;
class SolJsonArray;
class SolJsonValue;


class SolJson
{
public:
    using Expected = std::expected<QJsonValue, QString>;

    SolJson() = default;

    explicit SolJson(const Expected& r)
        : _value(r)
    {}

    explicit SolJson(const QByteArray& inJson)
        : SolJson{fromJson(inJson)}
    {}

    /** Parses inJson into an QJsonObject or QJsonArray. */
    static SolJson fromJson(const QByteArray& inJson);

    /** for object */
    SolJson value(const QString& inKey) const;

    /** for array */
    SolJson operator[](const qsizetype inIdx) const;

    const Expected& expected() const { return _value; };
    bool hasValue() const { return _value.has_value(); }

private:
    Expected _value;

};
#endif //SOLTRANSLATOR_SOLJSONOBJECT_H
