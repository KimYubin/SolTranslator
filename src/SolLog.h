// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLLOG_H
#define SOLTRANSLATOR_SOLLOG_H


class SolLog {
};

#define LOG_FUNC_LINE_INFO (QString(__FUNCTION__) + "(" + QString::fromStdString(std::to_string(__LINE__)) + ")")

#define solDebug qDebug() << LOG_FUNC_LINE_INFO


#endif //SOLTRANSLATOR_SOLLOG_H
