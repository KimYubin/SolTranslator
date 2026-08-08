// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENUMUTILS_H
#define SOLTRANSLATOR_ENUMUTILS_H

#include "SolUtilibrary.h"

#include <QString>

#include <magic_enum.hpp>

#include <type_traits>


namespace Sol
{
template <typename EnumType>
concept IsEnum = std::is_enum_v<EnumType>;

/**
 * Static cast the enum class member to a base type value.
 *
 * @tparam EnumType enum class Only
 */
template <IsEnum EnumType>
constexpr std::underlying_type_t<EnumType> enumToInt(EnumType inE) noexcept
{
    return static_cast<std::underlying_type_t<EnumType>>(inE);
}


/**
 * Convert enum to QString.
 */
template <IsEnum EnumType>
QString enumToQStr(const EnumType inDefaultVal)
{
    return qStrFromStdView(magic_enum::enum_name<EnumType>(inDefaultVal));
}


/**
 * Convert QString to enum.
 * 
 * @param inString Source string
 * @param inDefaultVal \a Default value to use if inString is invalid.
 */
template <IsEnum EnumType>
EnumType qStrToEnum(const QString& inString, const EnumType inDefaultVal)
{
    return magic_enum::enum_cast<EnumType>(inString.toStdString()).value_or(inDefaultVal);
}
} // namespace Sol 


#endif //SOLTRANSLATOR_ENUMUTILS_H
