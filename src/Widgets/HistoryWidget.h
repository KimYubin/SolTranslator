// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYWIDGET_H
#define SOLTRANSLATOR_HISTORYWIDGET_H

#include "ISolWidget.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

#include <QDateTime>


class LayoutTextEdit;
class HistoryCacheData;
class HistoryModel;
class QSplitter;
class QGridLayout;
class HistoryListView;

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
    void reTranslate() const;
    void viewPopup() const;

    using ExpectedHistory = Expected<const HistoryCacheData*>;
    ExpectedHistory getHistoryData(const QModelIndex& inCurIdx) const;

private:
    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    HistoryListView* _historyListView;
    HistoryModel* _historyListModel;
    LayoutTextEdit* _selectedTextEdit;

    qreal _listScrollBarRatio = 0.0;

    qint64 _currentTimelineId = -1;
    QDateTime _currentTimeStamp;

    TextCategory _currentTextCategory = TextCategory::TargetText;
};


#endif //SOLTRANSLATOR_HISTORYWIDGET_H
