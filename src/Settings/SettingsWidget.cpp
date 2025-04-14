//
// Created by YubinKim on 25/03/20 목.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingsWidget.h" resolved

#include "SettingsWidget.h"

#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QStyledItemDelegate>

#include "../FinTranslatorCore.h"
#include "../FinTranslatorMainWidget.h"

#include "../../ui/ui_SettingsWidget.h"

SettingsWidget::SettingsWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent, Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    , _finCore(inFinCore)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->themeButton, &QPushButton::clicked, this, &SettingsWidget::applyTheme);

    show();
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
