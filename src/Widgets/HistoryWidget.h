// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYWIDGET_H
#define SOLTRANSLATOR_HISTORYWIDGET_H

#include "ISolWidget.h"
#include "SolTypes.h"

#include <QDateTime>

#include <expected>


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

    using ExpectedHistory = std::expected<const HistoryCacheData*, QString>;
    ExpectedHistory getHistoryData(const QModelIndex& inCurIdx) const;


    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    HistoryListView* _historyListView;
    HistoryModel* _historyListModel;
    LayoutTextEdit* _selectedTextEdit;

    qreal _listScrollBarRatio = 0.0;

    TextType _currentTextType = TextType::TargetText;

    qint64 _currentTimelineId = -1;
    QDateTime _currentTimeStamp;
};


#endif //SOLTRANSLATOR_HISTORYWIDGET_H

