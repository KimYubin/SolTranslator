#include "fintranslator.h"
#include "../ui/ui_fintranslator.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMenu>


#include "ConfigManager.h"
#include "FinTypes.h"
#include "GlobalHotKeyManager.h"
#include "TranslateManager.h"


FinTranslator::FinTranslator(QWidget* parent) : QWidget(parent), ui(new Ui::FinTranslator)
{
    ui->setupUi(this);
    
    setLayout(ui->mainLayout);

    loadSettings();

    translateManager    = new TranslateManager(this);
    globalHotKeyManager = new GlobalHotKeyManager(this);

    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &FinTranslator::iconActivated);

    setIcon();

    trayIcon->show();
}

FinTranslator::~FinTranslator()
{
    delete ui;
}

void FinTranslator::setVisible(bool visible)
{
    if (visible)
    {
        activateWindow();
    }

    miniToTrayAction->setEnabled(visible);
    restoreAction->setEnabled(visible == false);

    QWidget::setVisible(visible);
}

void FinTranslator::closeEvent(QCloseEvent* event)
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

void FinTranslator::onSimpleTranslate(const QString& InOriginText)
{
    translateManager->translateSimple(InOriginText, LangType::AUTO, LangType::ko);
}

void FinTranslator::on_findButton_clicked()
{
    loadAPI();

    const QString orignText = ui->plainTextEditOrigin->toPlainText();

    translateManager->translateText(ui->plainTextEditTranslate, &QPlainTextEdit::setPlainText, orignText, LangType::en, LangType::ko);
}

void FinTranslator::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        showNormal();
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

void FinTranslator::loadSettings()
{
    loadAPI();
}

void FinTranslator::loadAPI()
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

void FinTranslator::createActions()
{
    miniToTrayAction = new QAction(tr("Mi&nimize"), this);
    connect(miniToTrayAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void FinTranslator::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(miniToTrayAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setVisible(true);
}

void FinTranslator::setIcon()
{
    QIcon icon = QIcon(":/img/icon_img.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
