// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "OptionSpec.h"

#include <QVariant>

QVariant OptionSpec::getDefaultValue() const
{
    switch (getOptionSpecType())
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
    case Type::StringSaver:
        return std::get<StringSaver>(defaultValue).defaultString;
    case Type::Combo:
        // return std::get<int>(defaultValue);
    default: ;
    }
    return {};
}
