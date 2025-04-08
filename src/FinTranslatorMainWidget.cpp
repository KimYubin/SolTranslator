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
#include <qdir.h>

#include "SettingsWidget.h"
#include "TextEditTranslateWidget.h"
#include "TranslateManager.h"

FinTranslatorMainWidget::FinTranslatorMainWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent), _finCore(inFinCore), ui(new Ui::FinTranslatorMainWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    // ~======================
    // button binding

    // addTab에서 부모 추가되므로, 부모추가 금지.
    _textEditTranslate = new TextEditTranslateWidget(_finCore);
    _settingsWidget    = new SettingsWidget(_finCore);

    const std::array stkIdxList = {     // std::array<idx, size>
        ui->mainStackedWidget->addWidget(_textEditTranslate)
      , ui->mainStackedWidget->addWidget(new QWidget())
      , ui->mainStackedWidget->addWidget(_settingsWidget)
    };
    const std::array buttonList = {    // std::array<QPushButton*, size>
        ui->button_0_TextTab
      , ui->button_1_dummy
      , ui->button_9_setting
    };
    static_assert(stkIdxList.size() == buttonList.size(), "not matching buttons and widgets.");


    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);

    for (int idx = 0; idx < stkIdxList.size(); ++idx)
    {
        buttonList[idx]->setCheckable(true);
        _buttonGroup->addButton(buttonList[idx], stkIdxList[idx]); // 비순서 임의 id 지정가능.
    }

    connect(_buttonGroup, &QButtonGroup::idClicked, this, [=](const int inButtonId)
    {
        ui->mainStackedWidget->setCurrentIndex(inButtonId);
    });

    // ~====================
    // button icon
    ui->button_9_setting->setIcon(QIcon(":/img/settings_gear_img"));
    
    // ~====================
    // tray icon
    createActions();
    createTrayIcon();
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    _trayIcon->show();

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

    _miniToTrayAction->setEnabled(visible);
    _restoreAction->setEnabled(visible == false);

    QWidget::setVisible(visible);
}

void FinTranslatorMainWidget::applyTheme(const QString& inThemeName)
{
    QString rtPrefixPath = "../resource/theme/" + inThemeName;
    const QDir rtThemeDir(rtPrefixPath, {"*.qss"}, QDir::Name, QDir::Files);
    if (rtThemeDir.exists() == false)
    {
        qDebug() << "no rt theme" << rtPrefixPath;
    }
    QStringList rtFiles = rtThemeDir.entryList();


    QString qrcPrefixPath = ":/theme/" + inThemeName;
    const QDir qrcThemeDir(qrcPrefixPath, {"*.qss"}, QDir::Name, QDir::Files);
    if (qrcThemeDir.exists() == false)
    {
        qWarning() << "qrc theme path is not valid" << qrcPrefixPath;
    }
    QStringList qrcFiles = qrcThemeDir.entryList();


    // 런타임 테마가 있다면 해당 테마 우선 사용.
    QString prefixPath;
    QStringList sheetFileList;
    if (qrcFiles == rtFiles)
    {
        prefixPath    = std::move(rtPrefixPath);
        sheetFileList = std::move(rtFiles);
    }
    else
    {
        qDebug() << "rt theme list is different from the existing theme list." << rtPrefixPath;
        prefixPath    = std::move(qrcPrefixPath);
        sheetFileList = std::move(qrcFiles);
    }

    QString newStyleSheet;
    for (const QString& sheetFileName : sheetFileList)
    {
        QFile file(prefixPath + "/" + sheetFileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream stream(&file);
            newStyleSheet += stream.readAll() + "\n";
            file.close();
        }
    }
    if (newStyleSheet.isEmpty() == false)
    {
        qApp->setStyleSheet(newStyleSheet);
        QWidgetList allWidgetList = qApp->allWidgets();
        for (QWidget* childWidget : allWidgetList)
        {
            childWidget->repaint();
        }
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
    if (_trayIcon->isVisible())
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
    _miniToTrayAction = new QAction(tr("Mi&nimize"), this);
    connect(_miniToTrayAction, &QAction::triggered, this, &QWidget::hide);

    _restoreAction = new QAction(tr("&Restore"), this);
    connect(_restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void FinTranslatorMainWidget::createTrayIcon()
{
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_miniToTrayAction);
    _trayIconMenu->addAction(_restoreAction);
    _trayIconMenu->addSeparator();
    _trayIconMenu->addAction(_quitAction);

    _trayIcon = new QSystemTrayIcon(this);
    setIcon();
    _trayIcon->setContextMenu(_trayIconMenu);
    _trayIcon->setVisible(true);
    _trayIcon->setToolTip(tr("FinTranslator"));
}

void FinTranslatorMainWidget::setIcon()
{
    QIcon icon = QIcon(":/img/icon_img");
    _trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
