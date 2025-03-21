//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FinTranslatorMainWidget.h" resolved

#include "FinTranslatorMainWidget.h"

#include <iostream>

#include "../ui/ui_FinTranslatorMainWidget.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMenu>


#include "ConfigManager.h"
#include <qevent.h>
#include <qstyle.h>
#include <qtabbar.h>
#include <QButtonGroup>

#include "SettingsWidget.h"
#include "TextEditTranslateWidget.h"
#include "TranslateManager.h"

FinTranslatorMainWidget::FinTranslatorMainWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent), finCore(inFinCore), ui(new Ui::FinTranslatorMainWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    // ~======================
    // tab button, instead of tab bar.
    // addTab에서 부모 추가되므로, 부모추가 금지.
    textEditTranslate = new TextEditTranslateWidget(finCore);
    settingsWidget    = new SettingsWidget(finCore);

    const int textEditTabIdx = ui->mainTabWidget->addTab(textEditTranslate, tr("Text"));
    const int settingTabIdx  = ui->mainTabWidget->addTab(settingsWidget, tr("Settings"));

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    ui->button_0_TextTab->setCheckable(true);
    ui->button_1->setCheckable(true);
    ui->button_9_setting->setCheckable(true);

    buttonGroup->addButton(ui->button_0_TextTab, 0);
    buttonGroup->addButton(ui->button_1, 1);
    buttonGroup->addButton(ui->button_9_setting, 9);

    connect(ui->button_0_TextTab, &QPushButton::clicked, this, [=]() { ui->mainTabWidget->setCurrentIndex(textEditTabIdx); });
    connect(ui->button_9_setting, &QPushButton::clicked, this, [=]() { ui->mainTabWidget->setCurrentIndex(settingTabIdx); });


    // ~====================
    // tray icon
    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    setIcon();

    trayIcon->show();

    applyTheme();

    connect(ui->pushButton, &QPushButton::clicked, this, &FinTranslatorMainWidget::applyTheme);
    
    setWindowTitle(tr("FinTranslator"));
}

FinTranslatorMainWidget::~FinTranslatorMainWidget()
{
    delete ui;
}

void FinTranslatorMainWidget::setVisible(bool visible)
{
    if (visible)
    {
        activateWindow();
    }

    miniToTrayAction->setEnabled(visible);
    restoreAction->setEnabled(visible == false);

    QWidget::setVisible(visible);
}

void FinTranslatorMainWidget::applyTheme()
{
    QFile theme("../resource/theme/dark.qss");
    if (theme.exists() == false)
    {
        theme.setFileName(":/theme/dark.qss");
    }

    if (theme.exists())
    {
        theme.open(QFile::ReadOnly | QFile::Text);

        QTextStream themeStream(&theme);
        const QString themeString = themeStream.readAll();
        if (themeString.isEmpty() == false)
        {
            qApp->setStyleSheet(themeString);
        }

        for (QWidget* childWidget : qApp->allWidgets())
        {
            childWidget->repaint();
        }
    }
    else
    {
        printf("Unable to set stylesheet, file not found\n");
    }
}

QString FinTranslatorMainWidget::applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors)
{
    QString res = templateTheme;
    for (const auto& [colorName, colorValue] : colors)
    {
        res.replace("${" + colorName + "}", colorValue);
    }
    return res;
}

void FinTranslatorMainWidget::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous() == false || isVisible() == false)
    {
        return;
    }
    if (trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}

void FinTranslatorMainWidget::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    case QSystemTrayIcon::Unknown:
        break;
    case QSystemTrayIcon::Context:
        break;
    default:
        ;
    }
}

void FinTranslatorMainWidget::createActions()
{
    miniToTrayAction = new QAction(tr("Mi&nimize"), this);
    connect(miniToTrayAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void FinTranslatorMainWidget::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(miniToTrayAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setVisible(true);
    trayIcon->setToolTip(tr("FinTranslator"));
}

void FinTranslatorMainWidget::setIcon()
{
    QIcon icon = QIcon(":/img/icon_img.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
