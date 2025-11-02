// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYWIDGET_H
#define FINTRANSLATOR_HISTORYWIDGET_H
#include "IFinWidget.h"


class QListView;
class QListWidget;
class MenuTextEdit;
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

    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    QListView* _historyList;
    MenuTextEdit* _selectedDetail;
};


#endif //FINTRANSLATOR_HISTORYWIDGET_H

