// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H
#define SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H

#include <QContextMenuEvent>
#include <QMenu>

/**
 * The SolMenuTextEdit class is a general text editor class
 * that provides a rounded corner context menu.
 *
 * @tparam BaseTextEdit requires virtual contextMenuEvent().
 */
template <typename BaseTextEdit>
class SolContextMenuEventWidget : public BaseTextEdit
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


#endif //SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H
