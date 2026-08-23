// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ToolTipData.h"

#include "Managers/ConfigManager.h"


namespace Sol
{
const char* ToolTipData::propertyName = "SolToolTipData";

namespace
{
QString toolTipShortcutToString(const QString& inToolTip, const QKeySequence& inKey)
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
    if (isVisible == false)
    {
        return "";
    }

    return isOnToolTip ? toolTip : offToolTip.has_value() ? offToolTip.value() : "";
}

QString ToolTipData::toolTipShortcutString() const
{
    if (isVisible == false)
    {
        return "";
    }

    QString res = toolTipString();

    if (solConfig.isShowToolTipShortcut())
    {
        return toolTipShortcutToString(res, shortcut);
    }

    return res;
}
} // namespace Sol
