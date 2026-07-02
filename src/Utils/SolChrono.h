// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLCHRONO_H
#define SOLTRANSLATOR_SOLCHRONO_H
#include "SolLog.h"

#include <chrono>

template <class _Type, template <class...> class _Template>
constexpr bool Is_specialization_v = false;
template <template <class...> class _Template, class... _Types>
constexpr bool Is_specialization_v<_Template<_Types...>, _Template> = true;
template <class _Ty>
constexpr bool Is_duration_v = Is_specialization_v<_Ty, std::chrono::duration>;

class SolChrono
{
    using time_point_hiRes = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
    SolChrono()
        : _start(clockNow())
        , _prev(_start)
    {}

    template <typename Duration = std::chrono::milliseconds, std::enable_if_t<Is_duration_v<Duration>, int> = 0>
    auto lapTime()
    {
        time_point_hiRes cur = clockNow();
        auto res = std::chrono::duration_cast<Duration>(cur - _prev);
        _prev = std::move(cur);
        return res;
    }

    template <typename Duration = std::chrono::milliseconds, std::enable_if_t<Is_duration_v<Duration>, int> = 0>
    auto splitTime()
    {
        time_point_hiRes cur = clockNow();
        auto res = std::chrono::duration_cast<Duration>(cur - _start);
        _prev = std::move(cur);
        return res;
    }

private:
    static time_point_hiRes clockNow()
    {
        return std::chrono::high_resolution_clock::now();
    }

    time_point_hiRes _start;
    time_point_hiRes _prev;
};


#endif //SOLTRANSLATOR_SOLCHRONO_H
