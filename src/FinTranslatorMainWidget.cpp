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

#include "TextEditTranslateWidget.h"
#include "TranslateManager.h"

FinTranslatorMainWidget::FinTranslatorMainWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent), finCore(inFinCore), ui(new Ui::FinTranslatorMainWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    // addTab에서 부모 추가되므로, 부모추가 금지. 
    textEditTranslate = new TextEditTranslateWidget(finCore);
    ui->mainTabWidget->addTab(textEditTranslate, "TranslateText");

    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    setIcon();

    trayIcon->show();

    applyTheme();

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
    QFile theme(":/theme/dark.qss");
    if (!theme.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        theme.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&theme);
        setStyleSheet(ts.readAll());
    }
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
    trayIcon->setToolTip("FinTranslator");
}

void FinTranslatorMainWidget::setIcon()
{
    QIcon icon = QIcon(":/img/icon_img.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
