// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYLISTVIEW_H
#define SOLTRANSLATOR_HISTORYLISTVIEW_H

#include "SolTypes.h"
#include "SolUtilibrary.h"

#include <QListView>


class HistoryListView : public QListView
{
    Q_OBJECT

    // HistoryListDelegate
    SOL_QSS_COLOR(itemTextColor)
    SOL_QSS_COLOR(itemSelectionTextColor)
    SOL_QSS_COLOR(itemHoverTextColor)
    SOL_QSS_COLOR(itemDisableColor)

public:
    explicit HistoryListView(QWidget* parent = nullptr);
    ~HistoryListView() override;

    QColor getItemColor(const Sol::ItemColorRole inColorRole) const;
};

#endif //SOLTRANSLATOR_HISTORYLISTVIEW_H
