//
// Created by YubinKim on 25/04/14 월.
//
#include <QString>

#include "EngineOptionWidget.h"

#include "../ConfigManager.h"
#include "../FinTranslatorCore.h"
#include "../Widgets/FinTranslatorMainWidget.h"

#include "ui_EngineOptionWidget.h"

EngineOptionWidget::EngineOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::EngineOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    ui->themeButton->setCheckable(false);
    QString api = ConfigManager::get().getAPIKey(EngineType::OpenAI);
    if (api.isEmpty() == false)
    {
        QString asterisk = QString(api.size(), '*');
        ui->apiInputLine->setPlaceholderText(asterisk);
    }
    connect(ui->apiInputLine, &QLineEdit::textEdited, this, [](const QString& inStr)
    {
        ConfigManager::get().setAPIKey(EngineType::OpenAI, inStr);
    });

    connect(ui->themeButton, &QPushButton::clicked, this, &EngineOptionWidget::applyTheme);
}

EngineOptionWidget::~EngineOptionWidget()
{
    delete ui;
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
