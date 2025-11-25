// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_FINLOG_H
#define FINTRANSLATOR_FINLOG_H


class FinLog {
};

#define LOG_FUNC_LINE_INFO (QString(__FUNCTION__) + "(" + QString::fromStdString(std::to_string(__LINE__)) + ")")

#define finDebug qDebug() << LOG_FUNC_LINE_INFO


#endif //FINTRANSLATOR_FINLOG_H
