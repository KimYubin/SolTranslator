// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLLOG_H
#define SOLTRANSLATOR_SOLLOG_H


class SolLog {
};

#if defined(_MSC_VER) || defined(__clang__)
#define FUNCTION_NAME __FUNCTION__
#elif defined(__GNUC__)
#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

#define LOG_FUNC_LINE_INFO (QString(FUNCTION_NAME) + "(" + QString::fromStdString(std::to_string(__LINE__)) + ")")

#define solDebug qDebug() << LOG_FUNC_LINE_INFO


#endif //SOLTRANSLATOR_SOLLOG_H
