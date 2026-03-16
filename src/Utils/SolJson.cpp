// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolJson.h"

#include <QJsonArray>


SolJson SolJson::fromJson(const QByteArray& inJson)
{
    QJsonParseError parseError;
    const QJsonDocument rootDoc = QJsonDocument::fromJson(inJson, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        return SolJson{std::unexpected{"Json parse error: " + parseError.errorString() + "\nJson: " + inJson}};
    }

    if (rootDoc.isArray())
    {
        return SolJson{rootDoc.array()};
    }

    if (rootDoc.isObject())
    {
        return SolJson{rootDoc.object()};
    }

    return SolJson{std::unexpected{"JsonDocument is empty."}};
}


SolJson SolJson::value(const QString& inKey) const
{
    if (hasValue() == false)
    {
        return SolJson{_expected};
    }

    if (_expected->isObject() == false)
    {
        return SolJson{std::unexpected("not an object: " + inKey)};
    }

    const QJsonValue val = _expected->toObject().value(inKey);
    if (val == QJsonValue::Undefined)
    {
        return SolJson{std::unexpected("not detected key: " + inKey)};
    }

    return SolJson{val};
}

SolJson SolJson::operator[](const qsizetype inIdx) const
{
    if (hasValue() == false)
    {
        return SolJson{_expected};
    }

    if (_expected->isArray() == false)
    {
        return SolJson{std::unexpected("not an array")};
    }

    auto arr = _expected->toArray();

    if (inIdx < 0 || arr.size() <= inIdx)
    {
        return SolJson{std::unexpected("out of range. arr size: " + QString::number(arr.size()) + ". index: " + QString::number(inIdx) + ".")};
    }

    return SolJson{arr[inIdx]};
}

bool SolJson::toBool(const bool defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toBool();
}

int SolJson::toInt(const int defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toInt();
}

qint64 SolJson::toInteger(const qint64 defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toInteger();
}

double SolJson::toDouble(const double defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toDouble();
}

QString SolJson::toString() const
{
    if (hasValue() == false)
    {
        return QString();
    }
    return _expected.value().toString();
}

QString SolJson::toString(const QString& defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toString();
}

QAnyStringView SolJson::toStringView(const QAnyStringView defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toStringView();
}

QJsonArray SolJson::toArray() const
{
    if (hasValue() == false)
    {
        return QJsonArray();
    }
    return _expected.value().toArray();
}

QJsonArray SolJson::toArray(const QJsonArray& defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toArray();
}

QJsonObject SolJson::toObject() const
{
    if (hasValue() == false)
    {
        return QJsonObject();
    }
    return _expected.value().toObject();
}

QJsonObject SolJson::toObject(const QJsonObject& defaultValue) const
{
    if (hasValue() == false)
    {
        return defaultValue;
    }
    return _expected.value().toObject();
}
