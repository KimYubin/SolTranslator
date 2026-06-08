// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolButton.h"

#include "SolToolTip.h"
#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Types/SolTypes.h"


void SolButton::setCheckIcon(const QString& inOnFileName, const QString& inOffFileName)
{
    QIcon checkIcon;
    checkIcon.addFile(inOnFileName, QSize(), QIcon::Normal, QIcon::On);
    checkIcon.addFile(inOffFileName, QSize(), QIcon::Normal, QIcon::Off);

    setIcon(checkIcon);
}


void SolButton::setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey)
{
    _toolTip = inToolTip;
    _offToolTip.reset();

    setShortcut(inKey);

    SolToolTip::setBubbleToolTip(this, Sol::toolTipShortcut(inToolTip, inKey));
}

void SolButton::setToolTipAction(const QString& inToolTip, const Action inAction)
{
    setToolTipShortcut(inToolTip, solConfig.shortcut(inAction));
}


void SolButton::setCheckToolTipShortcut(const QString& inOnToolTip
                                      , const QString& inOffToolTip
                                      , const QKeySequence& inKey)
{
    _toolTip    = inOnToolTip;
    _offToolTip = inOffToolTip;

    setShortcut(inKey);

    SolToolTip::setCheckableButtonToolTip(this
                                              , Sol::toolTipShortcut(inOnToolTip, inKey)
                                              , Sol::toolTipShortcut(inOffToolTip, inKey));
}

void SolButton::setCheckToolTipAction(const QString& inOnToolTip
                                    , const QString& inOffToolTip
                                    , const Action inAction = Action::None)
{
    setCheckToolTipShortcut(inOnToolTip, inOffToolTip, solConfig.shortcut(inAction));
}

void SolButton::changeShortcut(const QKeySequence& inKey)
{
    if (_offToolTip.has_value())
    {
        setCheckToolTipShortcut(_toolTip, _offToolTip.value(), inKey);
    }
    else
    {
        setToolTipShortcut(_toolTip, inKey);
    }
}

void SolButton::setAction(const Action inAction)
{
    changeShortcut(solConfig.shortcut(inAction));
}
