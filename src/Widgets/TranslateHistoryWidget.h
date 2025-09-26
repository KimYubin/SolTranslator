// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
#define FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
#include "IFinWidget.h"


class QListView;
class QListWidget;
class MenuTextEdit;
class QSplitter;
class QGridLayout;

class TranslateHistoryWidget : public IFinWidget
{
    Q_OBJECT

public:
    explicit TranslateHistoryWidget(QWidget* parent = nullptr);
    ~TranslateHistoryWidget() override;

private:
    void setupUI();

    QGridLayout* _mainLayout;
    QSplitter* _splitter;
    QListView* _historyList;
    MenuTextEdit* _selectedDetail;
};


#endif //FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
