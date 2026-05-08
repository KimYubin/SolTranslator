// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H

#include "IOptionWidget.h"
#include "Types/SolExpected.hpp"

class QTabWidget;
class ITranslateEngine;
struct OptionData;
class SettingCard;
class EngineId;
class SolTranslatorCore;

class EngineOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit EngineOptionWidget(QWidget* parent = nullptr);
    ~EngineOptionWidget() override;

protected:
    void addEngineSettings(const ITranslateEngine* inEngine);

private:
    friend class EngineOptionPage;

    Expected<SettingCard*> stringSaverCard(QWidget* inParent
                                         , const EngineId& inEngineId
                                         , const OptionData& inOptData);

    Expected<SettingCard*> doubleSpinCard(QWidget* inParent
                                        , const EngineId& inEngineId
                                        , const OptionData& inOptData);

private:
    QTabWidget* _tabWidget;
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
