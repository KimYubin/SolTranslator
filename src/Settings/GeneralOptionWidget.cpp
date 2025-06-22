//
// Created by YubinKim on 25/05/13 화.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GeneralOptionWidget.h" resolved

#include "GeneralOptionWidget.h"

#include <QPushButton>

#include "FinTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "SubWidgets/SettingCard.h"
#include "SubWidgets/SwitchButton.h"
#include "Widgets/FinTranslatorMainWidget.h"

#include "ui_GeneralOptionWidget.h"


GeneralOptionWidget::GeneralOptionWidget(QWidget* parent)
    : IOptionWidget(parent)
    , ui(new Ui::GeneralOptionWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    ui->mainLayout->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);

    // 시작시 실행
    SettingCard* startRunCard = new SettingCard(new SwitchButton, ui->engineGroup);
    startRunCard->setHeader(tr("Run at startup"));
    startRunCard->setDescription(tr("시스템 시작 시 자동 실행"));
    startRunCard->getContent<SwitchButton>()->setChecked(ConfigManager::get().getStartRun());
    connect(startRunCard->getContent<SwitchButton>(), &QCheckBox::checkStateChanged, this, [](Qt::CheckState inState)
    {
        ConfigManager::get().setStartRun(inState == Qt::CheckState::Checked);
    });
    ui->verticalLayout->addWidget(startRunCard, 0, Qt::AlignmentFlag::AlignTop);


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
