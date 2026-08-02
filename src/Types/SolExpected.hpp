// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLEXPECTED_H
#define SOLTRANSLATOR_SOLEXPECTED_H

#include "Utils/EnumUtils.hpp"

#include <QDebug>
#include <QString>

#include <expected>

enum class ErrorCode
{
    None
  , Size
};

struct Error
{
    Error() = default;

    Error(const QString& inMessage)
        : code(ErrorCode::None), message(inMessage) {}

    Error(QString&& inMessage)
        : code(ErrorCode::None), message(std::move(inMessage)) {}

    Error(const ErrorCode inCode, const QString& inMessage)
        : code(inCode), message(inMessage) {}

    Error(const ErrorCode inCode, QString&& inMessage)
        : code(inCode), message(std::move(inMessage)) {}

    Error(const Error& inOther)     = default;
    Error(Error&& inOther) noexcept = default;

    Error& operator=(const Error& inOther)     = default;
    Error& operator=(Error&& inOther) noexcept = default;

    friend bool operator==(const Error& inLhs, const Error& inRhs);
    friend bool operator!=(const Error& inLhs, const Error& inRhs);

    friend QDebug operator<<(QDebug inDebug, const Error& inError);

    ErrorCode code;
    QString message;
};

// ~====================
// friend functions
inline bool operator==(const Error& inLhs, const Error& inRhs)
{
    return std::tie(inLhs.code, inLhs.message) == std::tie(inRhs.code, inRhs.message);
}

inline bool operator!=(const Error& inLhs, const Error& inRhs)
{
    return !(inLhs == inRhs);
}

inline QDebug operator<<(QDebug inDebug, const Error& inError)
{
    static constexpr QAnyStringView debugMsg{"Unexpected Error: Code %1, Message %2"};

    QDebugStateSaver saver(inDebug);
    inDebug.nospace() << debugMsg.arg(Sol::enumToQStr(inError.code), inError.message);
    return inDebug;
}

// ~====================
// Make unexpected
template <typename E = Error, typename... Args>
    requires std::is_constructible_v<E, Args...>
[[nodiscard]] std::unexpected<E> makeUnexpected(Args&&... inVals)
    noexcept(std::is_nothrow_constructible_v<E, Args...>)
{
    return std::unexpected<E>{E{std::forward<Args>(inVals)...}};
}


/**
 * A std::expected alias that defaults to the project's standard error type.
 */
template <typename T, typename E = Error>
using Expected = std::expected<T, E>;


template <typename T, typename E>
[[nodiscard]]
constexpr bool is_error(const std::expected<T, E>& inE) noexcept
{
    return !inE.has_value();
}

#endif //SOLTRANSLATOR_SOLEXPECTED_H
