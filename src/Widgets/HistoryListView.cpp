// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryListView.h"


namespace Sol
{
QColor HistoryListView::getItemColor(const Sol::ItemColorRole inColorRole) const
{
    switch (inColorRole)
    {
    case Sol::ItemTextColorRole:          return _itemTextColor;
    case Sol::ItemSelectionTextColorRole: return _itemSelectionTextColor;
    case Sol::ItemHoverTextColorRole:     return _itemHoverTextColor;
    case Sol::ItemDisableColorRole:       return _itemDisableColor;
    default: ;
    }
    return QColor();
}
} // namespace Sol
