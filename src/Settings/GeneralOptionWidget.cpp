//
// Created by YubinKim on 25/05/13 화.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GeneralOptionWidget.h" resolved

#include "GeneralOptionWidget.h"

#include <QPushButton>

#include "ui_GeneralOptionWidget.h"

#include "../ConfigManager.h"
#include "../FinTranslatorCore.h"

#include "../Widgets/FinTranslatorMainWidget.h"


GeneralOptionWidget::GeneralOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::GeneralOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    ui->startRunCheckBox->setChecked(ConfigManager::get().getStartRun());
    connect(ui->startRunCheckBox, &QCheckBox::checkStateChanged, this, [](Qt::CheckState inCheckState)
    {
        ConfigManager::get().setStartRun(inCheckState == Qt::CheckState::Checked);
    });

    // 테마 적용 버튼
    QPushButton* themeButton = new QPushButton("ThemeButton");
    themeButton->setCheckable(false);
    connect(themeButton, &QPushButton::clicked, this, []()
    {
        if (finCore->getFinMainWidget())
        {
            finCore->getFinMainWidget()->applyTheme();
        }
        else
        {
            qDebug() << "finMainWidget is invalid";
        }
    });
    ui->mainLayout->addWidget(themeButton);
}

GeneralOptionWidget::~GeneralOptionWidget()
{
    delete ui;
}


void GeneralOptionWidget::apply()
{
    IOptionWidget::apply();
}

void GeneralOptionWidget::cancel()
{
    IOptionWidget::cancel();
}

void GeneralOptionWidget::finish()
{
    IOptionWidget::finish();
}


// ~======================
// GeneralOption 
GeneralOption::GeneralOption()
{
    setDisplayName(tr("일반"));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new GeneralOptionWidget(); });
    setPriority(OptionPriority::GeneralOption);
}

GeneralOption::~GeneralOption()
{
}

const GeneralOption engineOption;
