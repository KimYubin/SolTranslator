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
#include <QScrollArea>

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

    ui->findEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    ui->optionLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // ~=====================
    // option setup
    // scrollArea
    QWidget* container       = new QWidget();
    QVBoxLayout* innerLayout = new QVBoxLayout(container);
    QScrollArea* scrollArea  = new QScrollArea(this);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    scrollArea->setAlignment(Qt::AlignTop);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->buttonLayout->addWidget(scrollArea);


    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);

    const std::vector<IOptionPage*> options = IOptionPage::sortedOptionsPages();
    for (IOptionPage* option : options)
    {
        QPushButton* selectButton = new QPushButton(option->getIcon(), option->getDisplayName(), container);
        selectButton->setProperty("selectButton", true);
        selectButton->setCheckable(true);
        selectButton->setFocusPolicy(Qt::TabFocus);
        selectButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        const int stkIdx = ui->optionStackedWidget->addWidget(option->getOptionWidget());
        _buttonGroup->addButton(selectButton, stkIdx);
        innerLayout->addWidget(selectButton);
        innerLayout->setAlignment(selectButton, Qt::AlignTop);
    }

    // 맨 위 쪽으로 정렬하기 위해 하단에 Spacer 추가
    innerLayout->addStretch(1);

    connect(_buttonGroup, &QButtonGroup::idClicked, this, [this](const int inButtonId)
    {
        ui->optionStackedWidget->setCurrentIndex(inButtonId);
    });

    _buttonGroup->button(0)->click();


    show();
}

SettingsWidget::~SettingsWidget()
{
    const std::unordered_set<IOptionPage*>& options = IOptionPage::allOptionsPages();
    for (IOptionPage* option : options)
    {
        option->finish();
    }
    
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
