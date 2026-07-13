// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLBASETEXTEDIT_H
#define SOLBASETEXTEDIT_H

#include "SolContextMenuEventWidget.h"
#include "SolSmoothScroll.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextEdit>

/**
 * The SolBaseTextEdit class is a general text editor class
 * that provides a rounded corner context menu and smooth scrolling.
 *
 * @tparam BaseTextEdit requires virtual contextMenuEvent().
 */
template <typename BaseTextEdit>
using SolBaseTextEdit = SolSmoothAbstractScrollArea<SolContextMenuEventWidget<BaseTextEdit>>;

using SolPlainTextEdit = SolBaseTextEdit<QPlainTextEdit>;
using SolTextEdit      = SolBaseTextEdit<QTextEdit>;
using SolTextBrowser   = SolBaseTextEdit<QTextBrowser>;

using SolLineEdit = SolContextMenuEventWidget<QLineEdit>;

#endif //SOLBASETEXTEDIT_H
