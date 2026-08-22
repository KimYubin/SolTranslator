// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLCHRONO_H
#define SOLTRANSLATOR_SOLCHRONO_H

#include "Types/SolConcepts.hpp"

#include <chrono>

namespace Sol
{

// SpecializedFrom<std::chrono::duration> Duration>
template <class Specialized>
concept ChronoDuration = Sol::SpecializedFrom<Specialized, std::chrono::duration>;

} // namespace Sol

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
     * Returns the elapsed time since the previous call to lapTime().
     *
     * @tparam Duration std::chrono::duration. The default is milliseconds, double.
     */
    template <Sol::ChronoDuration Duration = milli_double>
    auto lapTime()
    {
        time_point_hi_res cur = clockNow();

        auto res = std::chrono::duration_cast<Duration>(cur - _prev);
        _prev    = std::move(cur);

        return res;
    }

    /**
     * Returns the cumulative elapsed time from the initialization.
     *
     * @tparam Duration std::chrono::duration. The default is milliseconds, double.
     */
    template <Sol::ChronoDuration Duration = milli_double>
    auto elapsedTime() const
    {
        return std::chrono::duration_cast<Duration>(clockNow() - _start);
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
