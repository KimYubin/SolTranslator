//
// Created by YubinKim on 25/03/20 목.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingsWidget.h" resolved

#include "SettingsWidget.h"
#include "../ui/ui_SettingsWidget.h"


SettingsWidget::SettingsWidget(QWidget* parent) :
    QWidget(parent), ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}
