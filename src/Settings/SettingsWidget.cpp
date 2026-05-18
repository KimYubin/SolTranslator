// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SettingsWidget.h"

#include "ui_SettingsWidget.h"

#include "IOptionWidget.h"
#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Types/SolTypes.h"
#include "Utils/SolI18n.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/SolMainWidget.h"

#include <QListWidget>
#include <QScrollArea>
#include <QShortcut>
#include <QStyledItemDelegate>

using Sol::i18n;

SettingsWidget::SettingsWidget(QWidget* parent)
    : ISolWidget(parent)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setWindowTitle(i18n(Tr::Settings));
    resize(640, 600);

    setLayout(ui->mainLayout);
    ui->mainLayout->setContentsMargins(0, 16, 0, 0);
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
    ui->listWidget->setFocusPolicy(Qt::TabFocus);

    const std::vector<QPointer<IOptionPage>> options = IOptionPage::sortedOptionsPages();
    for (const QPointer<IOptionPage>& option : options)
    {
        const int stkIdx = ui->optionStackedWidget->addWidget(option->getOptionWidget());

        QListWidgetItem* listItem = new QListWidgetItem(option->getIcon()
                                                      , option->getDisplayName()
                                                      , ui->listWidget);
        listItem->setData(stackIndexRole, stkIdx);
    }

    connect(ui->listWidget, &QListWidget::currentItemChanged, this, [this](const QListWidgetItem* current, const QListWidgetItem* prev)
    {
        ui->optionStackedWidget->setCurrentIndex(current->data(stackIndexRole).toInt());
        QWidget* curWidget = ui->optionStackedWidget->currentWidget();
        if (const IOptionWidget* curOptWidget = qobject_cast<IOptionWidget*>(curWidget))
        {
            ui->optionNameLabel->setText(curOptWidget->getOptionPage()->getDisplayName());
        }
    });

    ui->listWidget->setCurrentRow(0);
    ui->optionNameLabel->setFocus();

    QShortcut* closeShortcut = new QShortcut(this);
    closeShortcut->setKeys(QList{QKeySequence(Qt::Key_Escape), solConfig.shortcut(Action::SettingsClose)});
    connect(closeShortcut, &QShortcut::activated, this, &QWidget::close);


    show();

    solConfig.restoreWidgetGeometry(this);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &SettingsWidget::appQuitEvent);
}

SettingsWidget::~SettingsWidget()
{
    IOptionPage::allOptionsFinish();

    delete ui;
}

void SettingsWidget::closeEvent(QCloseEvent* event)
{
    solConfig.saveWidgetGeometry(this);
    ISolWidget::closeEvent(event);
}

void SettingsWidget::appQuitEvent()
{
    solConfig.saveWidgetGeometry(this);
}
