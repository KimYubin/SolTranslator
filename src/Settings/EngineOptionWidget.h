// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H
#include "IOptionWidget.h"

struct EngineOptionData;
class SettingCard;
class IAiEngine;
class EngineId;
class SolTranslatorCore;
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
    void setAiEngineUI(const IAiEngine* inEngine);

private:
    SettingCard* baseSettingCard(const EngineOptionData& inOptData, QWidget* inParent);
    SettingCard* doubleSpinCard(const EngineOptionData& inOptData, QWidget* inParent, const EngineId& inEngineId);

    friend class EngineOptionPage;
    Ui::EngineOptionWidget* ui;

};


class EngineOptionPage : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(EngineOptionPage)

public:
    EngineOptionPage();
    ~EngineOptionPage() override;
};


#endif //ENGINEOPTIONWIDGET_H
