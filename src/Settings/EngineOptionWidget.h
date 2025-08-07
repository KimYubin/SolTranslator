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
    void setEngineGroupUI();

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
    ~EngineOption() override;

};


#endif //ENGINEOPTIONWIDGET_H
