//
// Created by YubinKim on 25/03/20 목.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingsWidget.h" resolved

#include "SettingsWidget.h"

#include <QButtonGroup>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QStyledItemDelegate>

#include "IOptionWidget.h"

#include "../FinTranslatorCore.h"
#include "../Widgets/FinTranslatorMainWidget.h"

#include "ui_SettingsWidget.h"

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent, Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->findEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->optionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // ~=====================
    // option setup
    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);

    std::unordered_set<IOptionPage*> options = IOptionPage::allOptionsPages();
    for (IOptionPage* option : options)
    {
        QPushButton* selectButton = new QPushButton(option->getIcon(), option->getDisplayName(), this);
        selectButton->setProperty("selectButton", true);
        selectButton->setCheckable(true);
        selectButton->setFocusPolicy(Qt::TabFocus);
        selectButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        const int stkIdx = ui->optionStackedWidget->addWidget(option->getOptionWidget());
        _buttonGroup->addButton(selectButton, stkIdx);
        ui->buttonLayout->addWidget(selectButton);
        ui->buttonLayout->setAlignment(selectButton, Qt::AlignTop);
    }
    connect(_buttonGroup, &QButtonGroup::idClicked, this, [=](const int inButtonId)
    {
        ui->optionStackedWidget->setCurrentIndex(inButtonId);
    });

    _buttonGroup->button(0)->click();


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
