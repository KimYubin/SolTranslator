// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYLISTVIEW_H
#define SOLTRANSLATOR_HISTORYLISTVIEW_H

#include "SubWidgets/SolSmoothAbstractScrollArea.hpp"
#include "Types/SolTypes.h"
#include "Utils/SolUtilibrary.h"

#include <QListView>

using SolListView = SolSmoothAbstractScrollArea<QListView>;

class HistoryListView : public SolListView
{
    Q_OBJECT

    // HistoryListDelegate
    SOL_QSS_COLOR(itemTextColor)
    SOL_QSS_COLOR(itemSelectionTextColor)
    SOL_QSS_COLOR(itemHoverTextColor)
    SOL_QSS_COLOR(itemDisableColor)

public:
    explicit HistoryListView(QWidget* parent = nullptr) : SolListView(parent) {}
    ~HistoryListView() override = default;

    QColor getItemColor(const Sol::ItemColorRole inColorRole) const;
};

#endif //SOLTRANSLATOR_HISTORYLISTVIEW_H
