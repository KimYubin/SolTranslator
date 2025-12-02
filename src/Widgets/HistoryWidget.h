// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYWIDGET_H
#define SOLTRANSLATOR_HISTORYWIDGET_H
#include "ISolWidget.h"


class HistoryModel;
class QStringListModel;
class QListView;
class QListWidget;
class ResultTextEdit;
class QSplitter;
class QGridLayout;

class HistoryWidget : public ISolWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget* parent = nullptr);
    ~HistoryWidget() override;

private:
    void setupUI();
    void addEntry(const QString &name, const QString &address);

    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    QListView* _historyListView;
    HistoryModel* _historyListModel;
    ResultTextEdit* _selectedTextEdit;

    qreal _listScrollBarRatio = 0.0;
};


#endif //SOLTRANSLATOR_HISTORYWIDGET_H

