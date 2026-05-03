// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineOptionData.h"

#include <QVariant>

QVariant EngineOptionData::getDefaultValue() const
{
    switch (getOptionType())
    {
    case Type::None:
        return {};
    case Type::Int:
        return std::get<int>(defaultValue);
    case Type::Double:
        return std::get<double>(defaultValue);
    case Type::Bool:
        return std::get<bool>(defaultValue);
    case Type::SpinDataInt:
        return std::get<SpinData<int>>(defaultValue).value;
    case Type::SpinDataDouble:
        return std::get<SpinData<double>>(defaultValue).value;
    case Type::String:
        return std::get<QString>(defaultValue);
    case Type::Combo:
        // return std::get<int>(defaultValue);
    default: ;
    }
    return {};
}
