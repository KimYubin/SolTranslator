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
#include <QListWidget>

#include "IOptionWidget.h"

#include "../FinTranslatorCore.h"
#include "../Widgets/FinTranslatorMainWidget.h"

#include "ui_SettingsWidget.h"


enum
{
    stackIndexRole = Qt::ItemDataRole::UserRole + 1
  , OptionPageRole
};

SettingsWidget::SettingsWidget(QWidget* parent)
    : IFinWidget(parent)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setLayout(ui->mainLayout);
    ui->mainLayout->setContentsMargins(0, 16, 26, 0);
    ui->mainLayout->setHorizontalSpacing(24);
    ui->mainLayout->setVerticalSpacing(16);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->findEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    ui->optionNameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // ~=====================
    // option setup
    QFont qfont = ui->listWidget->font();
    qfont.setHintingPreference(QFont::PreferNoHinting);
    qfont.setStyleStrategy(QFont::PreferAntialias);
    ui->listWidget->setFont(qfont);
    ui->listWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->listWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    const std::vector<IOptionPage*> options = IOptionPage::sortedOptionsPages();
    for (IOptionPage* option : options)
    {
        const int stkIdx = ui->optionStackedWidget->addWidget(option->getOptionWidget());

        QListWidgetItem* listItem = new QListWidgetItem(option->getIcon()
                                                      , option->getDisplayName()
                                                      , ui->listWidget);
        listItem->setData(stackIndexRole, stkIdx);
    }

    connect(ui->listWidget, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem* previous)
    {
        ui->optionStackedWidget->setCurrentIndex(current->data(stackIndexRole).toInt());
        QWidget* curWidget = ui->optionStackedWidget->currentWidget();
        if (IOptionWidget* curOptionWidget = qobject_cast<IOptionWidget*>(curWidget))
        {
            ui->optionNameLabel->setText(curOptionWidget->getOptionPage()->getDisplayName());
        }
    });

    ui->listWidget->setCurrentRow(0);

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
