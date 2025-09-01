// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
#define FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
#include "IFinWidget.h"


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
};


#endif //FINTRANSLATOR_TRANSLATEHISTORYWIDGET_H
