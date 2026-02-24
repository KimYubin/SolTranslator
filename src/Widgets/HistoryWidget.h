// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYWIDGET_H
#define SOLTRANSLATOR_HISTORYWIDGET_H
#include <QListView>

#include "ISolWidget.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"


class LayoutTextEdit;
class HistoryCacheData;
class HistoryModel;
class QStringListModel;
class QListView;
class QListWidget;
class QSplitter;
class QGridLayout;

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

    QColor getItemColor(const sol::ItemColorRole inColorRole) const;
};

class HistoryWidget : public ISolWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget* parent = nullptr);
    ~HistoryWidget() override;

public:
signals:
    void exportHistoryData(const HistoryCacheData* inHistoryCache);

private:
    void setupUI();

    // export selected history data to TextEditTranslateWidget, etc.
    void exportSelectedHistoryData();

    void toggleTranslationText();

    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    HistoryListView* _historyListView;
    HistoryModel* _historyListModel;
    LayoutTextEdit* _selectedTextEdit;

    qreal _listScrollBarRatio = 0.0;

    sol::HistoryItemRole _currentTextRole = sol::TargetFullTextRole;
};


#endif //SOLTRANSLATOR_HISTORYWIDGET_H

