// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "CustomMenuTextEdit.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>

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
MenuTextEdit::MenuTextEdit(QWidget* parent) : QTextEdit(parent)
{
    _bottomWidget = new QFrame(this);
    _bottomWidget->setObjectName("bottomWidget");
    _layout       = new QHBoxLayout(_bottomWidget);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
}

MenuTextEdit::~MenuTextEdit() {}

void MenuTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    customMenu(event, createStandardContextMenu());
}

void MenuTextEdit::resizeEvent(QResizeEvent* event)
{
    QTextEdit::resizeEvent(event);

    _bottomWidget->adjustSize();
    QSize bottomSize = _bottomWidget->size();
    _bottomWidget->setGeometry(0, height() - bottomSize.height(), width(), bottomSize.height());
}

// ~============================================
// TextBrowser
MenuTextBrowser::MenuTextBrowser(QWidget* parent) : QTextBrowser(parent) {}
MenuTextBrowser::~MenuTextBrowser() {}

void MenuTextBrowser::contextMenuEvent(QContextMenuEvent* event)
{
    customMenu(event, createStandardContextMenu());
}
