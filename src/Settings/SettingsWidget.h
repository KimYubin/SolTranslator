//
// Created by YubinKim on 25/03/20 목.
//

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "Widgets/IFinWidget.h"

#include <QWidget>

QT_BEGIN_NAMESPACE

namespace Ui
{
class SettingsWidget;
}

QT_END_NAMESPACE

class SettingsWidget : public IFinWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget() override;

protected:
    virtual void closeEvent(QCloseEvent* event) override;

protected slots:
    void appQuitEvent() const;

private:
    void applyTheme();

    Ui::SettingsWidget* ui;
};


#endif //SETTINGSWIDGET_H
