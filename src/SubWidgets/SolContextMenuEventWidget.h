// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H
#define SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H

#include <QContextMenuEvent>
#include <QMenu>

template <typename T>
concept HasContextMenu = requires(T* t)
{
    { t->createStandardContextMenu() } -> std::same_as<QMenu*>;
};


/**
 * The SolContextMenuEventWidget class provides
 * a rounded corner context menu.
 *
 * @tparam BaseWidget requires createStandardContextMenu().
 */
template <HasContextMenu BaseWidget>
class SolContextMenuEventWidget : public BaseWidget
{
public:
    using BaseWidget::BaseWidget;

protected:
    void contextMenuEvent(QContextMenuEvent* inEvent) override
    {
        QMenu* menu = this->createStandardContextMenu();
        if (!menu)
        {
            return;
        }

        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->setAttribute(Qt::WA_TranslucentBackground);
        menu->setWindowFlag(Qt::FramelessWindowHint);
        menu->setWindowFlag(Qt::NoDropShadowWindowHint);
        menu->popup(inEvent->globalPos());

        inEvent->accept();
    }
};


#endif //SOLTRANSLATOR_SOLCONTEXTMENUEVENTWIDGET_H
