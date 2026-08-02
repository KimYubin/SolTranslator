// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef GeneralOptionWIDGET_H
#define GeneralOptionWIDGET_H

#include "IOptionWidget.h"


class GeneralOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit GeneralOptionWidget(QWidget* parent = nullptr);
    ~GeneralOptionWidget() override;

private:
    friend class GeneralOptionPage;

};


class GeneralOptionPage : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(GeneralOptionPage)

public:
    GeneralOptionPage();
    ~GeneralOptionPage() override;

};


#endif //GeneralOptionWIDGET_H
