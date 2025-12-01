// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYWIDGET_H
#define FINTRANSLATOR_HISTORYWIDGET_H
#include "IFinWidget.h"


class HistoryModel;
class QStringListModel;
class QListView;
class QListWidget;
class ResultTextEdit;
class QSplitter;
class QGridLayout;

class HistoryWidget : public IFinWidget
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


#endif //FINTRANSLATOR_HISTORYWIDGET_H

