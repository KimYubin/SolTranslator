// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLEXPECTED_H
#define SOLTRANSLATOR_SOLEXPECTED_H

#include <QString>

#include <expected>

enum class ErrorCode
{
    None
  , Size
};

struct Error
{
    Error(const QString& inMessage)
        : errorCode(ErrorCode::None)
        , message(inMessage)
    {}

    Error(const ErrorCode inErrorCode, const QString& inMessage)
        : errorCode(inErrorCode)
        , message(inMessage)
    {}

    ErrorCode errorCode;
    QString message;
};

inline std::unexpected<Error> makeUnexpected(QString&& inMessage)
{
    return std::unexpected{Error{std::move(inMessage)}};
}


template <typename T, typename E = QString>
using Expected = std::expected<T, E>;


template <typename T, typename E>
[[nodiscard]]
constexpr bool is_error(const std::expected<T, E>& e) noexcept
{
    return !e.has_value();
}

#endif //SOLTRANSLATOR_SOLEXPECTED_H
