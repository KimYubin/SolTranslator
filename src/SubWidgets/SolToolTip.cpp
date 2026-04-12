// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolToolTip.h"

#include "SolToolTipBallon.h"

#include <QAbstractButton>
#include <QEvent>


// ~==================================
// SolTooltipFilter

void SolTooltipFilter::setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip)
{
    static SolTooltipFilter* ins = new SolTooltipFilter();
    inTargetWidget->setToolTip(inToolTip);
    inTargetWidget->installEventFilter(ins);
}

void SolTooltipFilter::setCheckableButtonToolTip(QAbstractButton* inTargetWidget, const QString& inOnCheckToolTip, const QString& inOffCheckToolTip)
{
    const bool isChecked = (inTargetWidget->isCheckable() && inTargetWidget->isChecked());

    setBubbleToolTip(inTargetWidget, isChecked ? inOnCheckToolTip : inOffCheckToolTip);

    connect(inTargetWidget, &QAbstractButton::toggled, inTargetWidget, [inTargetWidget, inOnCheckToolTip, inOffCheckToolTip](const bool checked)
    {
        QString toolTip;
        if (checked)
        {
            toolTip = inOnCheckToolTip;
        }
        else
        {
            toolTip = inOffCheckToolTip;
        }
        inTargetWidget->setToolTip(toolTip);

        SolToolTipBallon::instance()->updateWidgetToolTip(inTargetWidget);
    });
}

SolTooltipFilter::SolTooltipFilter(QObject* parent) : QObject(parent)
{}

bool SolTooltipFilter::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::ToolTip:
    {
        // const QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        const QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget == nullptr)
        {
            return false;
        }

        const QString tooltipText = widget->toolTip();
        if (tooltipText.isEmpty() == false)
        {
            SolToolTipBallon::instance()->showToolTip(widget);
        }

        return true; // 기본 툴팁을 차단
    }
    case QEvent::Leave:
    case QEvent::Hide:
    case QEvent::Close:
    case QEvent::Quit:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
    {
        SolToolTipBallon::instance()->hideTipImmediately();
        break;
    }
    default: break;
    }

    return QObject::eventFilter(obj, event);
}
