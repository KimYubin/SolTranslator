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

namespace Sol
{
class SettingsWidget : public ISolWidget
{
    Q_OBJECT

    enum SettingsListItemRole
    {
        StackIndexRole = Qt::ItemDataRole::UserRole + 1
      , OptionPageRole
    };

public:
    explicit SettingsWidget(QWidget* inParent = nullptr);
    ~SettingsWidget() override;

protected:
    void closeEvent(QCloseEvent* inEvent) override;

protected slots:
    void appQuitEvent();

private:
    Ui::SettingsWidget* ui;
};


} // namespace Sol

#endif //SETTINGSWIDGET_H
