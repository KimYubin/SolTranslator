// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENUMUTILS_H
#define SOLTRANSLATOR_ENUMUTILS_H

#include "SolUtilibrary.h"

#include <QString>

#include <magic_enum.hpp>

#include <type_traits>


namespace Sol
{
/**
 * Static cast the enum class member to a base type value.
 * 
 * @tparam E enum class Only
 * @param e enum class value
 * @return static_cast<integer type>(e). std::underlying_type_t<E>(e)
 */
template <typename E>
constexpr std::enable_if_t<std::is_enum_v<E>, std::underlying_type_t<E>> EnumToInt(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}


/**
 * Convert enum to QString.
 */
template <typename EnumType>
    requires std::is_enum_v<EnumType>
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
template <typename EnumType>
    requires std::is_enum_v<EnumType>
EnumType qStrToEnum(const QString& inString, const EnumType inDefaultVal)
{
    return magic_enum::enum_cast<EnumType>(inString.toStdString()).value_or(inDefaultVal);
}
} // namespace Sol 


#endif //SOLTRANSLATOR_ENUMUTILS_H
