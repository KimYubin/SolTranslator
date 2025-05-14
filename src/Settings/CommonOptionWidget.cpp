//
// Created by YubinKim on 25/05/13 화.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CommonOptionWidget.h" resolved

#include "CommonOptionWidget.h"
#include "ui_CommonOptionWidget.h"


CommonOptionWidget::CommonOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::CommonOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    
}

CommonOptionWidget::~CommonOptionWidget()
{
    delete ui;
}


void CommonOptionWidget::apply()
{
    IOptionWidget::apply();
}

void CommonOptionWidget::cancel()
{
    IOptionWidget::cancel();
}

void CommonOptionWidget::finish()
{
    IOptionWidget::finish();
}


// ~======================
// CommonOption 
CommonOption::CommonOption()
{
    setDisplayName(tr("일반"));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new CommonOptionWidget(); });
}

CommonOption::~CommonOption()
{
}

const CommonOption engineOption;
