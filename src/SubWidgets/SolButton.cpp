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
    setShortcut(inKey);

    SolToolTip::setToolTipShortcut(this, inToolTip, inKey);
}

void SolButton::setToolTipAction(const QString& inToolTip, const Action inAction)
{
    setToolTipShortcut(inToolTip, solConfig.shortcut(inAction));
}


void SolButton::setCheckToolTipShortcut(const QString& inOnToolTip
                                      , const QString& inOffToolTip
                                      , const QKeySequence& inKey)
{
    setShortcut(inKey);

    SolToolTip::setCheckButtonToolTip(this, inOnToolTip, inOffToolTip, inKey);
}

void SolButton::setCheckToolTipAction(const QString& inOnToolTip
                                    , const QString& inOffToolTip
                                    , const Action inAction = Action::None)
{
    setCheckToolTipShortcut(inOnToolTip, inOffToolTip, solConfig.shortcut(inAction));
}

void SolButton::changeShortcut(const QKeySequence& inKey)
{
    setShortcut(inKey);

    SolToolTip::changeShortcut(this, inKey);
}

void SolButton::setAction(const Action inAction)
{
    changeShortcut(solConfig.shortcut(inAction));
}
