// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GeneralOptionWIDGET_H
#define GeneralOptionWIDGET_H

#include <QWidget>
#include "IOptionWidget.h"


class GeneralOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit GeneralOptionWidget(QWidget* parent = nullptr);
    ~GeneralOptionWidget() override;

protected:

private:
    friend class GeneralOption;

};


class GeneralOption : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(GeneralOption)

public:
    GeneralOption();
    ~GeneralOption() override;

};


#endif //GeneralOptionWIDGET_H
