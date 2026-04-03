// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ExJson.h"

#include <QJsonArray>
#include <QJsonObject>


ExJson ExJson::fromJson(const QByteArray& inJson)
{
    QJsonParseError parseError;
    const QJsonDocument rootDoc = QJsonDocument::fromJson(inJson, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        return ExJson{std::unexpected{"Json parse error: " + parseError.errorString() + "\nJson: " + inJson}};
    }

    if (rootDoc.isArray())
    {
        return ExJson{rootDoc.array()};
    }

    if (rootDoc.isObject())
    {
        return ExJson{rootDoc.object()};
    }

    return ExJson{std::unexpected{"JsonDocument is empty."}};
}


ExJson ExJson::value(const QString& inKey) const
{
    if (isError())
    {
        return ExJson{_expected};
    }

    if (_expected->isObject() == false)
    {
        return ExJson{std::unexpected("not an object: " + inKey)};
    }

    const QJsonValue val = _expected->toObject().value(inKey);
    if (val.isUndefined())
    {
        return ExJson{std::unexpected("not detected key: " + inKey)};
    }

    return ExJson{val};
}

ExJson ExJson::operator[](const qsizetype inIdx) const
{
    if (isError())
    {
        return ExJson{_expected};
    }

    if (_expected->isArray() == false)
    {
        return ExJson{std::unexpected("not an array")};
    }

    auto arr = _expected->toArray();

    if (inIdx < 0 || arr.size() <= inIdx)
    {
        return ExJson{std::unexpected("out of range. arr size: " + QString::number(arr.size()) + ". index: " + QString::number(inIdx) + ".")};
    }

    return ExJson{arr[inIdx]};
}


// ~============================================
/** QJsonValue interface */

bool ExJson::toBool(const bool inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toBool(inDefaultValue);
}

int ExJson::toInt(const int inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toInt(inDefaultValue);
}

qint64 ExJson::toInteger(const qint64 inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toInteger(inDefaultValue);
}

double ExJson::toDouble(const double inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toDouble(inDefaultValue);
}

QString ExJson::toString() const
{
    if (isError())
    {
        return QString();
    }
    return _expected->toString();
}

QString ExJson::toString(const QString& inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toString(inDefaultValue);
}

QAnyStringView ExJson::toStringView(const QAnyStringView inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toStringView(inDefaultValue);
}

QJsonArray ExJson::toArray() const
{
    if (isError())
    {
        return QJsonArray();
    }
    return _expected->toArray();
}

QJsonArray ExJson::toArray(const QJsonArray& inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toArray(inDefaultValue);
}

QJsonObject ExJson::toObject() const
{
    if (isError())
    {
        return QJsonObject();
    }
    return _expected->toObject();
}

QJsonObject ExJson::toObject(const QJsonObject& inDefaultValue) const
{
    if (isError())
    {
        return inDefaultValue;
    }
    return _expected->toObject(inDefaultValue);
}
