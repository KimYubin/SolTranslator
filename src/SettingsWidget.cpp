//
// Created by YubinKim on 25/03/20 목.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingsWidget.h" resolved

#include "SettingsWidget.h"
#include "../ui/ui_SettingsWidget.h"
#include <QPushButton>
#include <QListView>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QPointer>

#include "FinTranslatorCore.h"
#include "FinTranslatorMainWidget.h"



SettingsWidget::SettingsWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent)
    , finCore(inFinCore)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    connect(ui->themeButton, &QPushButton::clicked, this, &SettingsWidget::applyTheme);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::applyTheme()
{
    if (finCore->getFinMainWidget())
    {
        finCore->getFinMainWidget()->applyTheme();
    }
    else
    {
        qDebug()<<"finMainWidget is invalid";
    }
}
