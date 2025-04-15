//
// Created by YubinKim on 25/04/14 월.
//
#include <QString>

#include "EngineOptionWidget.h"

#include "../FinTranslatorCore.h"
#include "../FinTranslatorMainWidget.h"

#include "../../ui/ui_EngineOptionWidget.h"

EngineOptionWidget::EngineOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::EngineOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    ui->themeButton->setCheckable(false);

    connect(ui->themeButton, &QPushButton::clicked, this, &EngineOptionWidget::applyTheme);
}

EngineOptionWidget::~EngineOptionWidget()
{
    
}

void EngineOptionWidget::apply()
{
    IOptionWidget::apply();
}

void EngineOptionWidget::cancel()
{
    IOptionWidget::cancel();
}

void EngineOptionWidget::finish()
{
    IOptionWidget::finish();
}

void EngineOptionWidget::applyTheme()
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


// ~======================
// EngineOption 
EngineOption::EngineOption()
{
    setDisplayName(tr("번역 엔진"));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new EngineOptionWidget(); });
}

EngineOption::~EngineOption()
{
}

const EngineOption engineOption;
