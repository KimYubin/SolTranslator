// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "Widgets/ISolWidget.h"


QT_BEGIN_NAMESPACE

namespace Ui
{
class SettingsWidget;
}

QT_END_NAMESPACE

class SettingsWidget : public ISolWidget
{
    Q_OBJECT

    enum SettingsListItemRole
    {
        stackIndexRole = Qt::ItemDataRole::UserRole + 1
      , OptionPageRole
    };

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget() override;

protected:
    virtual void closeEvent(QCloseEvent* event) override;

protected slots:
    void appQuitEvent();

private:
    Ui::SettingsWidget* ui;
};


#endif //SETTINGSWIDGET_H
