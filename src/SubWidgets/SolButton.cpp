// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolButton.h"

#include "SolToolTip.h"

namespace
{
QString toolTipShortcut(const QString& inToolTip, const QKeySequence& inKey)
{
    if (inKey.isEmpty())
    {
        return inToolTip;
    }

    return inToolTip + "(" + inKey.toString() + ")";
}
}

void SolButton::setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey)
{
    setShortcut(inKey);

    SolTooltipFilter::setBubbleToolTip(this, toolTipShortcut(inToolTip, inKey.toString()));
}

void SolButton::setCheckIcon(const QString& inOnFileName, const QString& inOffFileName)
{
    QIcon checkIcon;
    checkIcon.addFile(inOffFileName, QSize(), QIcon::Normal, QIcon::Off);
    checkIcon.addFile(inOnFileName, QSize(), QIcon::Normal, QIcon::On);
    setIcon(checkIcon);
}

void SolButton::setCheckToolTipShortcut(const QString& inOnToolTip
                                      , const QString& inOffToolTip
                                      , const QKeySequence& inKey)
{
    setShortcut(inKey);

    SolTooltipFilter::setCheckableButtonToolTip(this
                                              , toolTipShortcut(inOnToolTip, inKey.toString())
                                              , toolTipShortcut(inOffToolTip, inKey.toString()));
}

void SolButton::changeShortcut(const QKeySequence& inKey)
{
    setShortcut(inKey);

    SolTooltipFilter::setBubbleToolTip(this, toolTipShortcut(toolTip(), inKey.toString()));
}
