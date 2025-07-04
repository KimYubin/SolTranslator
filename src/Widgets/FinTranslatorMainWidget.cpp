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
#include <QTimer>

#include <qevent.h>
#include <qtabbar.h>

#include "FinTranslatorCore.h"
#include "TextEditTranslateWidget.h"

#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "Settings/SettingsWidget.h"

#include "Widgets/ui_FinTranslatorMainWidget.h"


FinTranslatorMainWidget::FinTranslatorMainWidget(QWidget* parent)
    : IFinWidget(parent)
    , ui(new Ui::FinTranslatorMainWidget)
{
    qApp->setQuitOnLastWindowClosed(false);

    setWindowTitle(tr("FinTranslator"));

    ui->setupUi(this);

    setLayout(ui->mainLayout);

    _finIcon = QIcon(":/img/icon_img");
    qApp->setWindowIcon(_finIcon);

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

    QWidget::setVisible(visible);
}

void FinTranslatorMainWidget::showSettingsWidget()
{
    if (_settingsWidget.isNull())
    {
        _settingsWidget = new SettingsWidget();
    }
    else
    {
        if (_settingsWidget->isMinimized())
        {
            _settingsWidget->showNormal();
        }
        if (_settingsWidget->isHidden())
        {
            _settingsWidget->show();
        }

        _settingsWidget->raise();
        _settingsWidget->activateWindow();
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
    // 클릭시 마우스 위치 저장합니다.
    // 좌클릭과 아이콘 활성화 사이에 커서가 움직여도, 클릭 당시 위치에 메뉴를 생성합니다. 
    _prevMousePos = QCursor::pos();

    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        // 더블클릭과 구분을 위해 50ms 추가
        _doubleClickTimer->start(QApplication::doubleClickInterval() + 50);
        break;
    case QSystemTrayIcon::DoubleClick:
        _doubleClickTimer->stop();
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
        popupTrayMenu();
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

    _settingAction = new QAction(tr("&Settings"), this);
    connect(_settingAction, &QAction::triggered, this, &FinTranslatorMainWidget::showSettingsWidget);
    
    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void FinTranslatorMainWidget::createTrayIcon()
{
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_miniToTrayAction);
    _trayIconMenu->addAction(_restoreAction);
    _trayIconMenu->addAction(_settingAction);
    _trayIconMenu->addSeparator();
    _trayIconMenu->addAction(_quitAction);

    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setIcon(_finIcon);
    _trayIcon->setContextMenu(_trayIconMenu);
    _trayIcon->setVisible(true);
    _trayIcon->setToolTip(tr("FinTranslator"));


    _doubleClickTimer = new QTimer(this);
    _doubleClickTimer->setInterval(QApplication::doubleClickInterval() + 50);
    _doubleClickTimer->setSingleShot(true);
    connect(_doubleClickTimer, &QTimer::timeout, this, [this]()
    {
        popupTrayMenu();
    });

    connect(_trayIcon, &QSystemTrayIcon::activated, this, &FinTranslatorMainWidget::iconActivated);

    _trayIcon->show();
}

void FinTranslatorMainWidget::popupTrayMenu()
{
    if (_trayIcon && _trayIcon->contextMenu())
    {
        // 메뉴 사이즈 계산 유도.
        _trayIcon->contextMenu()->show();

        const QScreen* cursorScreen = qApp->screenAt(_prevMousePos);
        const QScreen* targetScreen = cursorScreen ? cursorScreen : qApp->primaryScreen();

        const QRect availableGeo = targetScreen ? targetScreen->availableGeometry() : QRect();
        const QSize contextSize  = _trayIcon->contextMenu()->size();

        QPoint popupPos = _prevMousePos;
        popupPos.rx() -= (contextSize.width() / 2); // 마우스 위치에 팝업 중앙이 오도록 조정.
        QRect popupGeo = QRect(popupPos, contextSize);

        /** 사용가능 영역 안쪽으로 이동. 커서 위 혹은, 시스템 영역에 겹치지 않도록 조정. */
        if (availableGeo.left() > popupGeo.left())
        {
            popupGeo.moveLeft(availableGeo.left());
        }
        if (availableGeo.top() > popupGeo.top())
        {
            popupGeo.moveTop(availableGeo.top());
        }
        if (availableGeo.right() < popupGeo.right())
        {
            popupGeo.moveRight(availableGeo.right());
        }
        if (availableGeo.bottom() < popupGeo.bottom())
        {
            popupGeo.moveBottom(availableGeo.bottom());
        }

        _trayIcon->contextMenu()->popup(popupGeo.topLeft());
    }
}
