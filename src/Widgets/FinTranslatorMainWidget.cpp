//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FinTranslatorMainWidget.h" resolved

#include "FinTranslatorMainWidget.h"

#include <QButtonGroup>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QTextStream>
#include <qevent.h>
#include <qtabbar.h>

#include "../ConfigManager.h"
#include "../FinTranslatorCore.h"

#include "../Settings/SettingsWidget.h"
#include "TextEditTranslateWidget.h"
#include "../TranslateManager.h"

#include "Widgets/ui_FinTranslatorMainWidget.h"


FinTranslatorMainWidget::FinTranslatorMainWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::FinTranslatorMainWidget)
{
    qApp->setQuitOnLastWindowClosed(false);

    setWindowTitle(tr("FinTranslator"));

    ui->setupUi(this);

    setLayout(ui->mainLayout);

    // ~======================
    // button binding
    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);

    auto bindButton = [this](QPushButton* button, QWidget* childWidget)
    {
        button->setCheckable(true);
        button->setFocusPolicy(Qt::TabFocus);

        const int stkIdx = ui->mainStackedWidget->addWidget(childWidget);
        _buttonGroup->addButton(button, stkIdx);
    };

    _textEditTranslate = new TextEditTranslateWidget();
    bindButton(ui->button_0_TextTab, _textEditTranslate);

    QWidget* dummyWidget = new QWidget();
    bindButton(ui->button_1_dummy, dummyWidget);

    connect(_buttonGroup, &QButtonGroup::idClicked, this, [this](const int inButtonId)
    {
        ui->mainStackedWidget->setCurrentIndex(inButtonId);
    });
    _buttonGroup->button(0)->click();
    _textEditTranslate->focusTextOrigin();


    // ~====================
    // setting button
    ui->button_9_setting->setCheckable(false);
    ui->button_9_setting->setFocusPolicy(Qt::TabFocus);
    connect(ui->button_9_setting, &QAbstractButton::clicked, this, &FinTranslatorMainWidget::showSettingsWidget);
    ui->button_9_setting->setIcon(QIcon(":/img/settings_gear_img"));


    // ~====================
    // tray icon
    createActions();
    createTrayIcon();
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    _trayIcon->show();

    applyTheme();

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

    if (_settingsWidget.isNull() == false)
    {
        _settingsWidget->setVisible(visible);
    }
    QWidget::setVisible(visible);
}

void FinTranslatorMainWidget::showSettingsWidget()
{
    if (_settingsWidget.isNull())
    {
        _settingsWidget = new SettingsWidget(this);
    }
    else
    {
        if (_settingsWidget->isMinimized())
        {
            _settingsWidget->showNormal();
        }
        _settingsWidget->show();
    }
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
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
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
        if (isMinimized())
        {
            if (isMaximized())
            {
                showMaximized();
            }
            else
            {
                showNormal();
            }
        }
        else
        {
            show();
        }
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
    _miniToTrayAction = new QAction(tr("Mi&nimize To Tray"), this);
    connect(_miniToTrayAction, &QAction::triggered, this, &QWidget::hide);

    _restoreAction = new QAction(tr("&Restore"), this);
    connect(_restoreAction, &QAction::triggered, this, &QWidget::show);

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
