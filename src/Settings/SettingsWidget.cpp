// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SettingsWidget.h"

#include <QButtonGroup>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QScrollArea>
#include <QListWidget>
#include <QShortcut>

#include "FinTranslatorCore.h"
#include "FinUtilibrary.h"

#include "Widgets/FinTranslatorMainWidget.h"
#include "IOptionWidget.h"

#include "ui_SettingsWidget.h"

#include "Managers/ConfigManager.h"


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
    setWindowTitle(tr("설정"));
    resize(640, 600);

    setLayout(ui->mainLayout);
    ui->mainLayout->setContentsMargins(0, 16, 26, 0);
    ui->mainLayout->setHorizontalSpacing(24);
    ui->mainLayout->setVerticalSpacing(16);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->findEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    ui->optionNameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // ~=====================
    // option setup
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

    QShortcut* closeShortcut = new QShortcut(this);
     closeShortcut->setKeys(QList{QKeySequence(Qt::Key_Escape), QKeySequence(Qt::CTRL | Qt::Key_W)});
    connect(closeShortcut, &QShortcut::activated, this, &QWidget::close);


    show();

    finConfig.restoreWidgetGeometry(this);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &SettingsWidget::appQuitEvent);
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

void SettingsWidget::closeEvent(QCloseEvent* event)
{
    finConfig.saveWidgetGeometry(this);
    IFinWidget::closeEvent(event);
}

void SettingsWidget::appQuitEvent() const
{
    finConfig.saveWidgetGeometry(this);
}
