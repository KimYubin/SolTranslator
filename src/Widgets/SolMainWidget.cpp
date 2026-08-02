// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolMainWidget.h"

#include "HistoryWidget.h"
#include "SolTranslatorCore.h"
#include "TextEditTranslateWidget.h"
#include "Managers/ConfigManager.h"
#include "Managers/GlobalHotKeyManager.h"
#include "Settings/SettingsWidget.h"
#include "SubWidgets/DropdownMenu.h"
#include "SubWidgets/EngineSelector.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolToolTip.h"
#include "SubWidgets/SolTrayIcon.h"
#include "Types/EngineId.h"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/ui_SolMainWidget.h"

#include <QButtonGroup>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <QTimer>

#include <vector>

using Sol::i18n;

SolMainWidget::SolMainWidget(QWidget* parent)
    : ISolWidget(parent)
    , ui(new Ui::SolMainWidget)
{
    qApp->setQuitOnLastWindowClosed(false);

    ui->setupUi(this);

    setWindowTitle(i18n(Tr::Sol_Translator));

    setLayout(ui->mainLayout);

    _solIcon = QIcon(":/img/icon_img");
    qApp->setWindowIcon(_solIcon);

    ui->tabBarLayout->setSpacing(20);

    std::vector<QWidget*> tabOrderList;
    tabOrderList.push_back(ui->mainStackedWidget);

    // ~======================
    // button binding
    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);


    auto bindButton = [this, &tabOrderList](QPushButton* button, QWidget* childWidget)
    {
        button->setCheckable(true);
        button->setFocusPolicy(Qt::TabFocus);
        ui->tabBarLayout->addWidget(button, 0, Qt::AlignLeft);

        const int stkIdx = ui->mainStackedWidget->addWidget(childWidget);
        _buttonGroup->addButton(button, stkIdx);
        tabOrderList.push_back(button);
    };

    // 텍스트 번역
    _textEditTranslate = new TextEditTranslateWidget();

    _textButton = new SolButton(this);
    _textButton->setObjectName("_textButton");
    _textButton->setText(i18n(Tr::Text));
    _textButton->setAction(Action::TextTab);
    _textButton->setIcon(QIcon(":/img/text_caret_cursor"));
    ui->tabBarLayout->addWidget(_textButton, 0, Qt::AlignLeft);

    bindButton(_textButton, _textEditTranslate);


    // 문서 번역
    QLabel* docTranslateWidget = new QLabel(i18n(Tr::Preparing));
    docTranslateWidget->setAlignment(Qt::AlignCenter);

    _docButton = new SolButton(this);
    _docButton->setObjectName("_docButton");
    _docButton->setText(i18n(Tr::Document));
    _docButton->setAction(Action::DocumentTab);
    _docButton->setIcon(QIcon(":/img/document_img"));
    ui->tabBarLayout->addWidget(_docButton, 0, Qt::AlignLeft);

    bindButton(_docButton, docTranslateWidget);

    // 번역 기록
    HistoryWidget* historyWidget = new HistoryWidget();

    _historyButton = new SolButton(this);
    _historyButton->setObjectName("_historyButton");
    _historyButton->setText(i18n(Tr::History));
    _historyButton->setIcon(QIcon(":/img/history_img"));
    _historyButton->setAction(Action::HistoryTab);
    ui->tabBarLayout->addWidget(_historyButton, 0, Qt::AlignLeft);

    bindButton(_historyButton, historyWidget);

    // apply an existing history to TextEditTranslateWidget.
    connect(historyWidget, &HistoryWidget::exportHistoryData, _textEditTranslate, &TextEditTranslateWidget::importExistingTranslation);

    connect(_buttonGroup, &QButtonGroup::idClicked, this, [this](const int inButtonId)
    {
        ui->mainStackedWidget->setCurrentIndex(inButtonId);
    });
    _buttonGroup->button(0)->click();
    _textEditTranslate->focusTextOrigin();


    // ~=========================
    // 번역 엔진 선택
    _engineSelector = new EngineSelector(this);
    _engineSelector->setToolTipAction(i18n(Tr::Select_Translation_Engine), Action::EngineSelector);
    _engineSelector->setCurrentIndexChanged([this](const int inIdx)
    {
        const QString payload = _engineSelector->itemData(inIdx).toString();
        const EngineId curEg  = EngineId{payload};
        solConfig.setCurrentEngineId(curEg);
    });

    ui->rightAlignLayout->insertWidget(1, _engineSelector, 0, Qt::AlignRight);


    // ~====================
    // setting button
    ui->settingsButton->setCheckable(false);
    ui->settingsButton->setText(i18n(Tr::Settings));
    ui->settingsButton->setIcon(QIcon(":/img/settings_gear_img"));
    ui->settingsButton->setShortcut(solConfig.shortcut(Action::SettingsOpen));
    ui->settingsButton->setFocusPolicy(Qt::TabFocus);
    connect(ui->settingsButton, &QAbstractButton::clicked, this, &SolMainWidget::showSettings);


    // ~====================
    // tray icon
    setupTrayIcon();

    setupShortcuts();

    solConfig.restoreWidgetGeometry(this);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &SolMainWidget::onAppQuitEvent);

    tabOrderList.insert(tabOrderList.end(), {_engineSelector, ui->settingsButton});

    for (int idx = 1; idx < tabOrderList.size(); ++idx)
    {
        setTabOrder(tabOrderList[idx - 1], tabOrderList[idx]);
    }


    solCore->manager<GlobalHotKeyManager>()->registerAction(
        Action::MainWidgetRaise
      , this
      , [this]() { showRaiseUp(); }
    );
}

SolMainWidget::~SolMainWidget()
{
    delete ui;
}

void SolMainWidget::setVisible(const bool visible)
{
    if (visible)
    {
        activateWindow();
    }

    emit visibleChanged(visible);

    QWidget::setVisible(visible);
}

void SolMainWidget::showSettings()
{
    if (_settingsWidget.isNull())
    {
        _settingsWidget = new SettingsWidget();
    }
    else
    {
        _settingsWidget->showRaiseUp();
    }
}

void SolMainWidget::closeEvent(QCloseEvent* event)
{
    if (solConfig.isFirstCloseToTray())
    {
        solConfig.setFirstCloseToTray();
        _trayIcon->showMessage(i18n(Tr::First_To_Tray_Noti)
                             , i18n(Tr::First_To_Tray_Message)
                             , QSystemTrayIcon::NoIcon
                             , 20'000);
    }

    solConfig.saveWidgetGeometry(this);
    hide();
    event->ignore();

    ISolWidget::closeEvent(event);
}

QMessageBox::StandardButton showNewMessageBox(const QMessageBox::Icon inIcon
                                            , const QString& inTitle
                                            , const QString& inText
                                            , const std::vector<std::pair<QString, QMessageBox::StandardButton>>& inButtons
                                            , const QMessageBox::StandardButton inDefaultButton
                                            , QWidget* inParent = nullptr
)
{
    QMessageBox msgBox(inIcon, inTitle, inText, QMessageBox::NoButton, inParent);

    const QDialogButtonBox* buttonBox = msgBox.findChild<QDialogButtonBox*>();
    Q_ASSERT(buttonBox != nullptr);

    for (auto& [buttonText, standButton] : inButtons)
    {
        QPushButton* button = msgBox.addButton(standButton);
        if (buttonText.isEmpty() == false)
        {
            button->setText(buttonText);
        }

        if (inDefaultButton == standButton)
        {
            msgBox.setDefaultButton(button);
        }
    }

    if (msgBox.exec() == -1)
    {
        return QMessageBox::Cancel;
    }

    return msgBox.standardButton(msgBox.clickedButton());
}

void SolMainWidget::quitApp()
{
    qApp->quit();

    return;

    // Confirm, if needs.
    const auto reply = showNewMessageBox(QMessageBox::Icon::NoIcon
                                       , i18n(Tr::Sol_Translator)
                                       , i18n(Tr::Confirm_Quit)
                                       , {{i18n(Tr::Quit), QMessageBox::Yes}, {i18n(Tr::Cancel), QMessageBox::Cancel}}
                                       , QMessageBox::Cancel);


    if (reply == QMessageBox::Yes)
    {
        qApp->quit();
    }
}

void SolMainWidget::onAppQuitEvent()
{
    solConfig.saveWidgetGeometry(this);
}

void SolMainWidget::iconActivated(const QSystemTrayIcon::ActivationReason inReason)
{
    // 클릭시 마우스 위치 저장합니다.
    // 좌클릭과 아이콘 활성화 사이에 커서가 움직여도, 클릭 당시 위치에 메뉴를 생성합니다.
    _prevMousePos = QCursor::pos();

    switch (inReason)
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

void SolMainWidget::setupTrayIcon()
{
    // trayIcon
    _trayIcon = new SolTrayIcon(_solIcon, this);
    _trayIcon->setToolTip(i18n(Tr::Sol_Translator));
    _trayIcon->show();

    connect(_trayIcon, &QSystemTrayIcon::activated, this, &SolMainWidget::iconActivated);

    // menu actions
    QAction* miniTray = _trayIcon->addAction(Tr::Tray_Minimize, this, &QWidget::hide);
    QAction* restore  = _trayIcon->addAction(Tr::Tray_Restore, this, &QWidget::show);
    connect(this, &SolMainWidget::visibleChanged, miniTray, &QAction::setEnabled);
    connect(this, &SolMainWidget::visibleChanged, restore, &QAction::setDisabled);

    _trayIcon->addAction(Tr::Tray_Translation, _textButton, [this](){show(); _textButton->click();});
    _trayIcon->addAction(Tr::Tray_History, _historyButton, [this](){show(); _historyButton->click();});
    _trayIcon->addAction(Tr::Tray_Settings, this, &SolMainWidget::showSettings);
    _trayIcon->addSeparator();
    _trayIcon->addAction(Tr::Tray_Quit, this, &SolMainWidget::quitApp, Qt::QueuedConnection);


    _doubleClickTimer = new QTimer(this);
    _doubleClickTimer->setInterval(QApplication::doubleClickInterval() + 50);
    _doubleClickTimer->setSingleShot(true);
    connect(_doubleClickTimer, &QTimer::timeout, this, &SolMainWidget::popupTrayMenu);
}

void SolMainWidget::setupShortcuts()
{
    const QShortcut* closeWidget = new QShortcut(solConfig.shortcut(Action::MainClose), this);
    connect(closeWidget, &QShortcut::activated, this, &QWidget::close);

    const QShortcut* nextTab = new QShortcut(solConfig.shortcut(Action::MainNextTab), this);
    connect(nextTab, &QShortcut::activated, this, [this]() { moveTab(TabMovement::Next); });

    const QShortcut* prvTab = new QShortcut(solConfig.shortcut(Action::MainPrevTab), this);
    connect(prvTab, &QShortcut::activated, this, [this]() { moveTab(TabMovement::Previous); });

}

void SolMainWidget::popupTrayMenu()
{
    if (_trayIcon && _trayIcon->contextMenu())
    {
        QMenu* contextMenu = _trayIcon->contextMenu();

        // 메뉴 사이즈 계산 유도.
        contextMenu->show();

        // todo: 트레이 아이콘 위치가 상단(화면 높이 절반 위인 경우)이면, 아이콘 아래로 메뉴가 열리게 변경
        // 트레이 아이콘 중앙 상단에, 메뉴 중앙 하단이 오도록 조정.
        const QRect trayGeo    = _trayIcon->geometry();
        const QPoint topCenter = trayGeo.topLeft() + QPoint(trayGeo.width() / 2, 0);
        const QSize menuSize   = contextMenu->size();
        const QPoint popupPos  = topCenter - QPoint(menuSize.width() / 2, menuSize.height());

        QRect popupGeo = QRect(popupPos, menuSize);

        // 사용가능 영역 안쪽으로 이동. 커서 위 혹은, 시스템 영역에 겹치지 않도록 조정.
        const QRect availableGeo = Sol::availableGeometryAt(_prevMousePos);
        popupGeo = Sol::moveToInside(availableGeo, popupGeo);

        contextMenu->popup(popupGeo.topLeft());
    }
}

void SolMainWidget::moveTab(const TabMovement inMovement)
{
    const int widgetCount = ui->mainStackedWidget->count();
    int moveIdx = ui->mainStackedWidget->currentIndex();

    if (inMovement == TabMovement::Next)
    {
        moveIdx += 1;
    }
    else
    {
        moveIdx += -1 + widgetCount;
    }

    moveIdx %= widgetCount;

    if (QAbstractButton* nextButton = _buttonGroup->button(moveIdx))
    {
        nextButton->click();
    }
}

