// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinTranslatorMainWidget.h"

#include <QButtonGroup>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <QStyle>
#include <QTextStream>
#include <QTimer>
#include <qevent.h>

#include "FinTranslatorCore.h"
#include "FinUtilibrary.h"
#include "TextEditTranslateWidget.h"
#include "SubWidgets/FinToolTip.h"

#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "Settings/SettingsWidget.h"

#include "SubWidgets/DropdownMenu.h"

#include "Widgets/ui_FinTranslatorMainWidget.h"


FinTranslatorMainWidget::FinTranslatorMainWidget(QWidget* parent)
    : IFinWidget(parent)
    , ui(new Ui::FinTranslatorMainWidget)
{
    qApp->setQuitOnLastWindowClosed(false);

    ui->setupUi(this);

    setWindowTitle(tr("FinTranslator"));

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

    // 텍스트 번역
    _textEditTranslate = new TextEditTranslateWidget();
    ui->textTabButton->setText(tr("텍스트"));
    ui->textTabButton->setIcon(QIcon(":/img/text_caret_cursor"));
    bindButton(ui->textTabButton, _textEditTranslate);

    // 문서 번역
    QLabel* docTranslateWidget = new QLabel(tr("준비 중"));
    docTranslateWidget->setAlignment(Qt::AlignCenter);
    ui->docTabButton->setText(tr("문서"));
    ui->docTabButton->setIcon(QIcon(":/img/document_img"));
    bindButton(ui->docTabButton, docTranslateWidget);


    connect(_buttonGroup, &QButtonGroup::idClicked, this, [this](const int inButtonId)
    {
        ui->mainStackedWidget->setCurrentIndex(inButtonId);
    });
    _buttonGroup->button(0)->click();
    _textEditTranslate->focusTextOrigin();


    // ~=========================
    // 번역 엔진 선택
    _engineSelector = new DropdownMenu(this);

    for (EngineType eg = EngineType::Default; eg != EngineType::Size; eg = static_cast<EngineType>(static_cast<int>(eg) + 1))
    {
        _engineSelector->addItem(EngineName::getName(eg), static_cast<int>(eg));
    }

    _engineSelector->setEditable(false);
    _engineSelector->setCurrentIndex(static_cast<int>(finConfig.getCurrentEngineType()));
    FinTooltipFilter::setBubbleToolTip(_engineSelector, tr("번역 엔진 선택"));

    connect(_engineSelector, &QComboBox::currentIndexChanged, this, [this](const int inIdx)
    {
        const int payload      = _engineSelector->itemData(inIdx).toInt();
        const EngineType curEg = static_cast<EngineType>(payload);
        finConfig.setCurrentEngineType(curEg);
    });

    ui->rightAlignLayout->insertWidget(1, _engineSelector, 0, Qt::AlignmentFlag::AlignRight);


    // ~====================
    // setting button
    ui->settingsButton->setCheckable(false);
    ui->settingsButton->setText(tr("설정"));
    ui->settingsButton->setFocusPolicy(Qt::TabFocus);
    connect(ui->settingsButton, &QAbstractButton::clicked, this, &FinTranslatorMainWidget::showSettingsWidget);
    ui->settingsButton->setIcon(QIcon(":/img/settings_gear_img"));


    // ~====================
    // tray icon
    createActions();
    createTrayIcon();


    QShortcut* closeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), this);
    connect(closeShortcut, &QShortcut::activated, this, &QWidget::close);

    finConfig.restoreWidgetGeometry(this);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &FinTranslatorMainWidget::onAppQuitEvent);

    setTabOrder(_engineSelector, _settingsWidget);
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

void FinTranslatorMainWidget::closeEvent(QCloseEvent* event)
{
    if (event->spontaneous() == false || isVisible() == false)
    {
        return;
    }
    if (_trayIcon->isVisible())
    {
        if (finConfig.isFirstCloseToTray())
        {
            finConfig.setFirstCloseToTray();
            _trayIcon->showMessage(tr("트레이로 최소화되었습니다.")
                                 , tr("Fin.번역기가 아직 실행 중입니다.\n"
                                       "아이콘을 클릭하여 다시 실행하거나, 종료할 수 있습니다.")
                                 , QSystemTrayIcon::NoIcon, 20'000);
        }
        finConfig.saveWidgetGeometry(this);
        hide();
        event->ignore();
    }
}

QMessageBox::StandardButton showNewMessageBox(QWidget* inParent
                                            , const QMessageBox::Icon inIcon
                                            , const QString& inTitle
                                            , const QString& inText
                                            , const std::vector<std::pair<QString, QMessageBox::StandardButton>>& inButtons
                                            , const QMessageBox::StandardButton inDefaultButton
)
{
    QMessageBox msgBox(inIcon, inTitle, inText, QMessageBox::NoButton, inParent);

    QDialogButtonBox* buttonBox = msgBox.findChild<QDialogButtonBox*>();
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

void FinTranslatorMainWidget::quitApp()
{
    const auto reply = showNewMessageBox(this
                                       , QMessageBox::Icon::Question
                                       , tr("Fin.Translator")
                                       , tr("정말 종료할까요?")
                                       , {{tr("종료"), QMessageBox::Yes}, {tr("취소"), QMessageBox::Cancel}}
                                       , QMessageBox::Cancel);


    if (reply == QMessageBox::Yes)
    {
        qApp->quit();
    }
    
}

void FinTranslatorMainWidget::onAppQuitEvent() const
{
    finConfig.saveWidgetGeometry(this);
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
    _miniToTrayAction = new QAction(tr("트레이로 최소화(&M)"), this);
    connect(_miniToTrayAction, &QAction::triggered, this, &QWidget::hide);

    _restoreAction = new QAction(tr("창 복원(&R)"), this);
    connect(_restoreAction, &QAction::triggered, this, &QWidget::show);

    _settingAction = new QAction(tr("설정(&S)"), this);
    connect(_settingAction, &QAction::triggered, this, &FinTranslatorMainWidget::showSettingsWidget);
    
    _quitAction = new QAction(tr("종료(&Q)"), this);
    connect(_quitAction, &QAction::triggered, this, &FinTranslatorMainWidget::quitApp, Qt::QueuedConnection);
}

void FinTranslatorMainWidget::createTrayIcon()
{
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->setAttribute(Qt::WA_TranslucentBackground);
    _trayIconMenu->setWindowFlag(Qt::FramelessWindowHint);
    _trayIconMenu->setWindowFlag(Qt::NoDropShadowWindowHint);
    _trayIconMenu->setObjectName("trayIconMenu");

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

        const QRect availableGeo = Fin::availableGeometryAt(_prevMousePos);
        const QSize contextSize  = _trayIcon->contextMenu()->size();

        QPoint popupPos = _prevMousePos;
        popupPos.rx() -= (contextSize.width() / 2); // 마우스 위치에 팝업 중앙이 오도록 조정.
        popupPos.ry() -= contextSize.height(); 
        QRect popupGeo = QRect(popupPos, contextSize);

        /** 사용가능 영역 안쪽으로 이동. 커서 위 혹은, 시스템 영역에 겹치지 않도록 조정. */
        popupGeo = Fin::moveToInside(availableGeo, popupGeo);

        _trayIcon->contextMenu()->popup(popupGeo.topLeft());
    }
}

