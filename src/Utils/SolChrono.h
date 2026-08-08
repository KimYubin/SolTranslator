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
    using time_point_hi_res = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using milli_double      = std::chrono::duration<double, std::milli>;

public:
    SolChrono()
        : _start(clockNow())
        , _prev(_start)
    {}

    /**
     * Returns the time interval between the last call to lapTime().
     *
     * @tparam Duration The default is milliseconds, double.
     */
    template <typename Duration = milli_double, std::enable_if_t<Is_duration_v<Duration>, int> = 0>
    auto lapTime()
    {
        time_point_hi_res cur = clockNow();
        auto res              = std::chrono::duration_cast<Duration>(cur - _prev);
        _prev                 = std::move(cur);
        return res;
    }

    /**
     * Returns the cumulative elapsed time from the beginning.
     *
     * @tparam Duration The default is milliseconds, double.
     */
    template <typename Duration = milli_double, std::enable_if_t<Is_duration_v<Duration>, int> = 0>
    auto elapsedTime()
    {
        time_point_hi_res cur = clockNow();
        auto res              = std::chrono::duration_cast<Duration>(cur - _start);
        _prev                 = std::move(cur);
        return res;
    }

private:
    static time_point_hi_res clockNow()
    {
        return std::chrono::high_resolution_clock::now();
    }

    time_point_hi_res _start;
    time_point_hi_res _prev;
};


#endif //SOLTRANSLATOR_SOLCHRONO_H
