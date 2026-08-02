// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLBASETEXTEDIT_H
#define SOLBASETEXTEDIT_H

#include "SolContextMenuEventWidget.h"
#include "SolSmoothAbstractScrollArea.hpp"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextEdit>

template <typename T>
concept HasTextInteractionFlags = requires(T* t)
{
    { t->textInteractionFlags() } -> std::same_as<Qt::TextInteractionFlags>;
};


/**
 * The SolBaseTextEdit class is a general text editor class
 * that provides a rounded corner context menu and smooth scrolling.
 *
 * @tparam BaseTextEdit requires textInteractionFlags().
 */
template <HasTextInteractionFlags BaseTextEdit>
class SolBaseTextEdit : public SolSmoothAbstractScrollArea<SolContextMenuEventWidget<BaseTextEdit>>
{
public:
    using Base = SolSmoothAbstractScrollArea<SolContextMenuEventWidget<BaseTextEdit>>;
    using Base::Base;

protected:
    void wheelEvent(QWheelEvent* event) override
    {
        const QPoint angleDelta = event->angleDelta();

        // Zoom
        if (!(this->textInteractionFlags().testFlag(Qt::TextEditable)))
        {
            if (event->modifiers().testFlag(Qt::ControlModifier))
            {
                float delta = angleDelta.y() / 120.f;
                this->zoomInF(delta);
                return;
            }
        }

        Base::wheelEvent(event);
        this->updateMicroFocus();
    }
};

using SolPlainTextEdit = SolBaseTextEdit<QPlainTextEdit>;
using SolTextEdit      = SolBaseTextEdit<QTextEdit>;
using SolTextBrowser   = SolBaseTextEdit<QTextBrowser>;

using SolLineEdit = SolContextMenuEventWidget<QLineEdit>;

#endif //SOLBASETEXTEDIT_H
