//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FinTranslatorMainWidget.h" resolved

#include "FinTranslatorMainWidget.h"
#include "../ui/ui_FinTranslatorMainWidget.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMenu>


#include "ConfigManager.h"
#include <qevent.h>

#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "TranslateManager.h"

FinTranslatorMainWidget::FinTranslatorMainWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent), finCore(inFinCore), ui(new Ui::FinTranslatorMainWidget)
{
    ui->setupUi(this);
    
    setLayout(ui->mainLayout);

    loadSettings();
    
    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    setIcon();

    trayIcon->show();

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

void FinTranslatorMainWidget::on_findButton_clicked()
{
    loadAPI();

    const QString orignText = ui->plainTextEditOrigin->toPlainText();

    finCore->getTranslateManager()->translateText(ui->plainTextEditTranslate, &QPlainTextEdit::setPlainText, orignText, LangType::en, LangType::ko);
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

void FinTranslatorMainWidget::loadSettings()
{
    loadAPI();
}

void FinTranslatorMainWidget::loadAPI()
{
    QString newAPI = ui->lineEdit_api->text();
    if (newAPI.isEmpty())
    {
        QString oldAPI = ConfigManager::get().getAPI();
        if (oldAPI.isEmpty() == false)
        {
            QString asteriskAPI = oldAPI.first(3) + "***...";
            ui->lineEdit_api->setText(asteriskAPI);
        }
    }
    else
    {
        if (newAPI.last(6) != "***...")
        {
            ConfigManager::get().setAPI(newAPI);
        }
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
    trayIcon->setToolTip("FinTranslatorMainWidget");
}

void FinTranslatorMainWidget::setIcon()
{
    QIcon icon = QIcon(":/img/icon_img.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
