// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYWIDGET_H
#define SOLTRANSLATOR_HISTORYWIDGET_H

#include "ISolWidget.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include <QDateTime>
#include <QListView>


class LayoutTextEdit;
class HistoryCacheData;
class HistoryModel;
class QStringListModel;
class QListView;
class QListWidget;
class QSplitter;
class QGridLayout;
// todo: cpp 옮기기
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

    TextType _currentTextType = TextType::TranslateText;

    qint64 _currentTimelineId = -1;
    QDateTime _currentTimeStamp;
};


#endif //SOLTRANSLATOR_HISTORYWIDGET_H

