//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FinTranslatorMainWidget.h" resolved

#include "FinTranslatorMainWidget.h"
#include "../ui/ui_FinTranslatorMainWidget.h"


FinTranslatorMainWidget::FinTranslatorMainWidget(QWidget* parent) : QWidget(parent), ui(new Ui::FinTranslatorMainWidget)
{
    ui->setupUi(this);
}

FinTranslatorMainWidget::~FinTranslatorMainWidget()
{
    delete ui;
}
