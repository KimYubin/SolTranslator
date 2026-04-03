// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLJSONOBJECT_H
#define SOLTRANSLATOR_SOLJSONOBJECT_H

#include <QJsonValue>

#include <expected>


/**
 * The ExJson class is a QJsonValue wrapper class
 * that provides safe error handling and chaining
 * based on 'std::expected'.
 */
class ExJson
{
public:
    using Expected = std::expected<QJsonValue, QString>;

    ExJson() = default;

    explicit ExJson(const Expected& r)
        : _expected(r)
    {}

    explicit ExJson(const QByteArray& inJson)
        : ExJson{fromJson(inJson)}
    {}

    /** Parses \a inJson into an QJsonObject or QJsonArray. */
    static ExJson fromJson(const QByteArray& inJson);

    /** for object */
    ExJson value(const QString& inKey) const;

    /** for array */
    ExJson operator[](const qsizetype inIdx) const;

    constexpr explicit operator bool() const noexcept { return _expected.has_value(); }
    bool hasValue() const { return _expected.has_value(); }
    bool isError() const { return (_expected.has_value() == false); }

    QString error() const { return _expected.has_value() ? "No error. exist value" : _expected.error(); }

    const Expected& expected() const { return _expected; }

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
