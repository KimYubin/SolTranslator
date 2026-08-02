// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHABSTRACTSCROLLAREA_HPP
#define SOLTRANSLATOR_SOLSMOOTHABSTRACTSCROLLAREA_HPP

#include "SolSmoothScrollBar.h"

#include <QAbstractScrollArea>
#include <QWheelEvent>

/**
 * The SolSmoothAbstractScrollArea class is a ScrollArea class
 * that provides smooth wheel scrolling.
 *
 * @tparam BaseType requires QAbstractScrollArea-derived.
 */
template <std::derived_from<QAbstractScrollArea> BaseType>
class SolSmoothAbstractScrollArea : public BaseType
{
public:
    using Base = BaseType;

    explicit SolSmoothAbstractScrollArea(QWidget* inParent = nullptr)
        : Base(inParent)
    {
        this->setHorizontalScrollBar(new SolSmoothScrollBar(this));
        this->setVerticalScrollBar(new SolSmoothScrollBar(this));
    }

protected:
    void wheelEvent(QWheelEvent* inEvent) override
    {
        const QPoint angleDelta = inEvent->angleDelta();
        bool isHorizontal = qAbs(angleDelta.x()) > qAbs(angleDelta.y());

        if (inEvent->modifiers().testFlag(Qt::ShiftModifier))
        {
            isHorizontal = !isHorizontal;
        }

        if (isHorizontal)
        {
            QCoreApplication::sendEvent(this->horizontalScrollBar(), inEvent);
        }
        else
        {
            QCoreApplication::sendEvent(this->verticalScrollBar(), inEvent);
        }
    }
};


#endif //SOLTRANSLATOR_SOLSMOOTHABSTRACTSCROLLAREA_HPP
