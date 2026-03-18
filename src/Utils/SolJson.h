// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLJSONOBJECT_H
#define SOLTRANSLATOR_SOLJSONOBJECT_H

#include <QJsonObject>
#include <QJsonValue>

#include <expected>


class SolJsonValueRef;
class SolJsonArray;
class SolJsonValue;


/**
 * The SolJson class is a QJsonValue wrapper class
 * that provides safe error handling and chaining.
 */
class SolJson
{
public:
    using Expected = std::expected<QJsonValue, QString>;

    SolJson() = default;

    explicit SolJson(const Expected& r)
        : _expected(r)
    {}

    explicit SolJson(const QByteArray& inJson)
        : SolJson{fromJson(inJson)}
    {}

    /** Parses \a inJson into an QJsonObject or QJsonArray. */
    static SolJson fromJson(const QByteArray& inJson);

    /** for object */
    SolJson value(const QString& inKey) const;

    /** for array */
    SolJson operator[](const qsizetype inIdx) const;

    bool hasValue() const { return _expected.has_value(); }
    constexpr explicit operator bool() const noexcept { return _expected.has_value(); }

    QString error() const { return _expected.has_value() ? "" : _expected.error(); };

    const Expected& expected() const { return _expected; };

    // ~============================================
    /** QJsonValue interface */
    bool toBool(const bool inDefaultValue = false) const;
    int toInt(const int inDefaultValue = 0) const;
    qint64 toInteger(const qint64 inDefaultValue = 0) const;
    double toDouble(const double inDefaultValue = 0) const;
    QString toString() const;
    QString toString(const QString& inDefaultValue) const;
    QAnyStringView toStringView(const QAnyStringView inDefaultValue = {}) const;
    QJsonArray toArray() const;
    QJsonArray toArray(const QJsonArray& inDefaultValue) const;
    QJsonObject toObject() const;
    QJsonObject toObject(const QJsonObject& inDefaultValue) const;

private:
    Expected _expected;
};

#endif //SOLTRANSLATOR_SOLJSONOBJECT_H
