// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLCONCEPTS_HPP
#define SOLTRANSLATOR_SOLCONCEPTS_HPP
#include <type_traits>

namespace Sol
{
template <typename _Specialized, template <typename...> class _Template>
struct IsSpecialization : std::false_type {};

template <template <typename...> class _Template, typename... _Types>
struct IsSpecialization<_Template<_Types...>, _Template> : std::true_type {};

template <typename _Specialized, template <typename...> class _Template>
concept SpecializedFrom = IsSpecialization<_Specialized, _Template>::value;
} // namespace Sol


#endif //SOLTRANSLATOR_SOLCONCEPTS_HPP
