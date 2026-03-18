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


// ~============================================
/** QJsonValue interface */

bool SolJson::toBool(const bool inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toBool(inDefaultValue);
}

int SolJson::toInt(const int inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toInt(inDefaultValue);
}

qint64 SolJson::toInteger(const qint64 inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toInteger(inDefaultValue);
}

double SolJson::toDouble(const double inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toDouble(inDefaultValue);
}

QString SolJson::toString() const
{
    if (hasValue() == false)
    {
        return QString();
    }
    return _expected->toString();
}

QString SolJson::toString(const QString& inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toString(inDefaultValue);
}

QAnyStringView SolJson::toStringView(const QAnyStringView inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toStringView(inDefaultValue);
}

QJsonArray SolJson::toArray() const
{
    if (hasValue() == false)
    {
        return QJsonArray();
    }
    return _expected->toArray();
}

QJsonArray SolJson::toArray(const QJsonArray& inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toArray(inDefaultValue);
}

QJsonObject SolJson::toObject() const
{
    if (hasValue() == false)
    {
        return QJsonObject();
    }
    return _expected->toObject();
}

QJsonObject SolJson::toObject(const QJsonObject& inDefaultValue) const
{
    if (hasValue() == false)
    {
        return inDefaultValue;
    }
    return _expected->toObject(inDefaultValue);
}
