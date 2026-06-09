// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ToolTipData.h"

#include "Managers/ConfigManager.h"


const char* ToolTipData::Name = "SolToolTipData";

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
} // anonymous namespace

QString ToolTipData::toolTipString() const
{
    return isOn ? toolTip : OffToolTip.has_value() ? OffToolTip.value() : "";
}

QString ToolTipData::toolTipShortcutString() const
{
    QString res = toolTipString();

    if (solConfig.isShowToolTipShortcut())
    {
        return toolTipShortcut(res, shortcut);
    }

    return res;
}
