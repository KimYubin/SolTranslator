// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H
#include "IOptionWidget.h"

class FinTranslatorCore;
QT_BEGIN_NAMESPACE
namespace Ui
{
class EngineOptionWidget;
}
QT_END_NAMESPACE

class EngineOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit EngineOptionWidget(QWidget* parent = nullptr);
    ~EngineOptionWidget() override;

protected:
    virtual void apply() override;
    virtual void cancel() override;
    virtual void finish() override;

private:
    friend class EngineOption;
    Ui::EngineOptionWidget* ui;
};



class EngineOption : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(EngineOption)

public:
    EngineOption();
    virtual ~EngineOption() override;

};


#endif //ENGINEOPTIONWIDGET_H
