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
        return SolJson{_value};
    }

    if (_value->isObject() == false)
    {
        return SolJson{std::unexpected("not an object: " + inKey)};
    }

    const QJsonValue val = _value->toObject().value(inKey);
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
        return SolJson{_value};
    }

    if (_value->isArray() == false)
    {
        return SolJson{std::unexpected("not an array")};
    }

    auto arr = _value->toArray();

    if (inIdx < 0 || arr.size() <= inIdx)
    {
        return SolJson{
            std::unexpected("out of range"
                "\n - index: " + QString::number(inIdx)
                + "\n - arr size: " + QString::number(arr.size()))
        };
    }

    return SolJson{arr[inIdx]};
}
