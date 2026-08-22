// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDEBUG_H
#define SOLTRANSLATOR_SOLDEBUG_H

#include <QDebug>

#define solDebug qDebug()

namespace Sol
{
template <typename T>
concept HasQDebugOperator = requires(QDebug inDebug, const T& inValue)
{
    { inDebug << inValue } -> std::convertible_to<QDebug>;
};

template <typename Range>
concept RangeQDebugElements
        = std::ranges::range<Range> && HasQDebugOperator<std::ranges::range_value_t<Range>>;


template <RangeQDebugElements Range>
inline QDebug debugRange(QDebug inDebug, const Range& inVec)
{
    QDebugStateSaver saver(inDebug);

    inDebug.nospace().noquote() << "\n";

    for (const auto& elem : inVec)
    {
        inDebug << elem << "\n";
    }

    return inDebug;
}
} // namespace Sol

#endif //SOLTRANSLATOR_SOLDEBUG_H
