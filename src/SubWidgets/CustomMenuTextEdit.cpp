// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "CustomMenuTextEdit.h"

#include <QMenu>

void customMenu(QContextMenuEvent* event, QMenu* menu)
{
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlag(Qt::FramelessWindowHint);
    menu->setWindowFlag(Qt::NoDropShadowWindowHint);
    menu->popup(event->globalPos());

    event->accept();
}

// ~============================================
// PlainTextEdit
MenuPlainTextEdit::MenuPlainTextEdit(QWidget* parent) : QPlainTextEdit(parent) {}
MenuPlainTextEdit::~MenuPlainTextEdit() {}

void MenuPlainTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    customMenu(event, createStandardContextMenu());
}

// ~============================================
// TextEdit
MenuTextEdit::MenuTextEdit(QWidget* parent) : QTextEdit(parent) {}
MenuTextEdit::~MenuTextEdit() {}

void MenuTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    customMenu(event, createStandardContextMenu());
}

// ~============================================
// TextBrowser
MenuTextBrowser::MenuTextBrowser(QWidget* parent) : QTextBrowser(parent) {}
MenuTextBrowser::~MenuTextBrowser() {}

void MenuTextBrowser::contextMenuEvent(QContextMenuEvent* event)
{
    customMenu(event, createStandardContextMenu());
}
