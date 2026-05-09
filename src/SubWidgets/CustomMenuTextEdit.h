// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef CUSTOMMENUTEXTEDIT_H
#define CUSTOMMENUTEXTEDIT_H

#include <QLineEdit>
#include <QMenu>
#include <QPlainTextEdit>
#include <QTextBrowser>

/**
 * The CustomMenuTextEdit class is a general text editor class
 * that provides a rounded corner context menu.
 *
 * @tparam BaseTextEdit requires virtual contextMenuEvent().
 */
template <typename BaseTextEdit>
class CustomMenuTextEdit : public BaseTextEdit
{
public:
    using BaseTextEdit::BaseTextEdit;

protected:
    virtual void contextMenuEvent(QContextMenuEvent* event) override
    {
        QMenu* menu = this->createStandardContextMenu();
        menu->setAttribute(Qt::WA_TranslucentBackground);
        menu->setWindowFlag(Qt::FramelessWindowHint);
        menu->setWindowFlag(Qt::NoDropShadowWindowHint);
        menu->popup(event->globalPos());

        event->accept();
    }
};

using MenuPlainTextEdit = CustomMenuTextEdit<QPlainTextEdit>;
using MenuTextEdit      = CustomMenuTextEdit<QTextEdit>;
using MenuTextBrowser   = CustomMenuTextEdit<QTextBrowser>;
using MenuLineEdit      = CustomMenuTextEdit<QLineEdit>;

#endif //CUSTOMMENUTEXTEDIT_H
