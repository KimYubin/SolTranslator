// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolToolTip.h"

#include "SolToolTipBallon.h"

#include <QAbstractButton>
#include <QEvent>


// ~==================================
// SolToolTipFilter

SolToolTipFilter::SolToolTipFilter(QObject* parent) : QObject(parent)
{}

void SolToolTipFilter::setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip)
{
    static SolToolTipFilter* ins = new SolToolTipFilter();
    inTargetWidget->setToolTip(inToolTip);
    inTargetWidget->installEventFilter(ins);
}

void SolToolTipFilter::setCheckableButtonToolTip(QAbstractButton* inTargetWidget, const QString& inOnCheckToolTip, const QString& inOffCheckToolTip)
{
    const bool isChecked = (inTargetWidget->isCheckable() && inTargetWidget->isChecked());

    setBubbleToolTip(inTargetWidget, isChecked ? inOnCheckToolTip : inOffCheckToolTip);

    connect(inTargetWidget, &QAbstractButton::toggled, inTargetWidget, [inTargetWidget, inOnCheckToolTip, inOffCheckToolTip](const bool checked)
    {
        const QString& toolTip = checked ? inOnCheckToolTip : inOffCheckToolTip;
        inTargetWidget->setToolTip(toolTip);

        SolToolTipBallon::instance()->updateWidgetToolTip(inTargetWidget);
    });
}

bool SolToolTipFilter::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::ToolTip:
    {
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
        SolToolTipBallon::instance()->hideToolTipImmediately();
        break;
    }
    default: break;
    }

    return QObject::eventFilter(obj, event);
}
