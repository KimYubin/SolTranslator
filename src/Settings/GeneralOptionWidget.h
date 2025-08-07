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
    virtual void apply() override;
    virtual void cancel() override;
    virtual void finish() override;

private:
    friend class GeneralOption;

};


class GeneralOption : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(GeneralOption)

public:
    GeneralOption();
    virtual ~GeneralOption() override;

};


#endif //GeneralOptionWIDGET_H
