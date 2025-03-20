//
// Created by YubinKim on 25/03/20 목.
//

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class SettingsWidget; }
QT_END_NAMESPACE

class SettingsWidget : public QWidget {
Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget() override;

private:
    Ui::SettingsWidget *ui;
};


#endif //SETTINGSWIDGET_H
