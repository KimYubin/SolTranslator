// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryListView.h"


HistoryListView::HistoryListView(QWidget* parent) : QListView(parent)
{}

HistoryListView::~HistoryListView()
{}

QColor HistoryListView::getItemColor(const Sol::ItemColorRole inColorRole) const
{
    switch (inColorRole)
    {
    case Sol::itemTextColorRole:          return _itemTextColor;
    case Sol::itemSelectionTextColorRole: return _itemSelectionTextColor;
    case Sol::itemHoverTextColorRole:     return _itemHoverTextColor;
    case Sol::itemDisableColorRole:       return _itemDisableColor;
    default: ;
    }
    return QColor();
}
