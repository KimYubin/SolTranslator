// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLBASETEXTEDIT_H
#define SOLBASETEXTEDIT_H

#include "SolContextMenuEventWidget.h"
#include "SolSmoothScrollArea.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextEdit>

// template <typename BaseTextEdit>
// using SolBaseTextEdit = SolSmoothScrollArea<SolContextMenuEventWidget<BaseTextEdit>>;

template <typename BaseTextEdit>
class SolBaseTextEdit : public SolSmoothScrollArea<SolContextMenuEventWidget<BaseTextEdit>>
{
public:
    using Base = SolSmoothScrollArea<SolContextMenuEventWidget<BaseTextEdit>>;
    using Base::Base;

protected:
    virtual void wheelEvent(QWheelEvent* event) override
    {
        const QPoint angleDelta = event->angleDelta();

        // Zoom
        if (!(Base::textInteractionFlags().testFlag(Qt::TextEditable)))
        {
            if (event->modifiers().testFlag(Qt::ControlModifier))
            {
                float delta = angleDelta.y() / 120.f;
                Base::zoomInF(delta);
                return;
            }
        }

        Base::wheelEvent(event);
    }
};

using SolPlainTextEdit = SolBaseTextEdit<QPlainTextEdit>;
using SolTextEdit      = SolBaseTextEdit<QTextEdit>;
using SolTextBrowser   = SolBaseTextEdit<QTextBrowser>;

using SolLineEdit = SolContextMenuEventWidget<QLineEdit>;

#endif //SOLBASETEXTEDIT_H
