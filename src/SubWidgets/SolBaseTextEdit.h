// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLBASETEXTEDIT_H
#define SOLBASETEXTEDIT_H

#include "SolContextMenuEventWidget.h"
#include "SolSmoothAbstractScrollArea.hpp"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextEdit>

namespace Sol
{

template <typename T>
concept HasTextInteractionFlags = requires(T* t)
{
    { t->textInteractionFlags() } -> std::same_as<Qt::TextInteractionFlags>;
};

} // namespace Sol


/**
 * The SolBaseTextEdit class is a general text editor class
 * that provides a rounded corner context menu and smooth scrolling.
 *
 * @tparam BaseTextEdit requires textInteractionFlags().
 */
template <Sol::HasTextInteractionFlags BaseTextEdit>
class SolBaseTextEdit : public SolSmoothAbstractScrollArea<SolContextMenuEventWidget<BaseTextEdit>>
{
public:
    using Base = SolSmoothAbstractScrollArea<SolContextMenuEventWidget<BaseTextEdit>>;
    using Base::Base;

protected:
    void wheelEvent(QWheelEvent* inEvent) override
    {
        const QPoint angleDelta = inEvent->angleDelta();

        // Zoom
        if (!(this->textInteractionFlags().testFlag(Qt::TextEditable)))
        {
            if (inEvent->modifiers().testFlag(Qt::ControlModifier))
            {
                float delta = angleDelta.y() / 120.f;
                this->zoomInF(delta);
                return;
            }
        }

        Base::wheelEvent(inEvent);
        this->updateMicroFocus();
    }
};

using SolPlainTextEdit = SolBaseTextEdit<QPlainTextEdit>;
using SolTextEdit      = SolBaseTextEdit<QTextEdit>;
using SolTextBrowser   = SolBaseTextEdit<QTextBrowser>;

using SolLineEdit = SolContextMenuEventWidget<QLineEdit>;

#endif //SOLBASETEXTEDIT_H
