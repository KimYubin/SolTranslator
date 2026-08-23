// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "PopupTranslateWidget.h"

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"
#include "SubWidgets/LoadingSpinner.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolToolTip.h"
#include "SubWidgets/SolWidgetFactory.h"
#include "Types/EngineId.h"
#include "Utils/SolAsync.hpp"
#include "Utils/SolDocument.h"
#include "Utils/SolI18n.h"
#include "Utils/SolDebug.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/ui_PopupTranslateWidget.h"

#include <QBoxLayout>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QSizeGrip>
#include <QTextDocument>
#include <QWindow>

#ifdef _WIN32
#include <qt_windows.h>
#endif


namespace Sol
{
PopupTranslateWidget::PopupTranslateWidget(QWidget* inParent)
    : ITranslateWidget(inParent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , _minSizeRatio(0.15, 0.15)
    , _maxSizeRatio(0.2, 0.65)
    , _fullSizeRatio(0.95, 0.95)
    , _centerPosRatio(0.85, 0.33)
    , _yPosMaxRatio((1.0 - _maxSizeRatio.height()) / 2.0)
    , ui(new Ui::PopupTranslateWidget)
{
    setWindowIcon(QIcon{":/img/icon_img"});
    setWindowTitle(i18n(Tr::Sol));

    // ~===========
    // config
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    // ~===========
    // ui
    setupUI();

    // 포커스 변경에 따른 그림자 on/off 제어. (그림자 성능)
    connect(qApp, &QApplication::focusChanged, this, &PopupTranslateWidget::detectFocusInOut);

    // ~======================
    // 애니메이션
    _animation = new QPropertyAnimation(this, "textEditSize", this); // setTextEditSize 함수 연결
    _animation->setDuration(250);
    _animation->setEasingCurve(QEasingCurve::OutQuad);
    connect(_animation, &QAbstractAnimation::finished, this, &PopupTranslateWidget::adjustSizeAfterAnimationFinished);

    calculateTextEditLayoutInfo();

    show();
    raise();
    activateWindow();

    setMouseTracking(true);

    // 리사이즈 영역에서 커서 모양 변경.
    // bgFrame에서 mouse move 이벤트를 부모(현재 위젯)로 전달하기 위해 이벤트 필터를 설치합니다.
    ui->verticalLayoutWidget->setGeometry(0, 0, 0, 0);
    ui->bgFrame->setMouseTracking(true);
    ui->bgFrame->installEventFilter(this);

    if (solConfig.isPopupTrWindowTemp())
    {
        changePopupMode();
    }
    else
    {
        changeNormalWindowMode();
    }

    // resizeEvent 유도를 위해 show 이후에 호출
    // animation start size 지정.
    ui->resultText->setFixedSize(20, 20);
    adjustSize();
    showTranslationPopup();
}

PopupTranslateWidget::~PopupTranslateWidget()
{
    qApp->removeEventFilter(this);

    delete ui;
}

bool PopupTranslateWidget::eventFilter(QObject* inObj, QEvent* inEvent)
{
    // 팝업모드에서 자동 종료
    if (inObj == qApp
        && _widgetModeFlags.testFlag(SolWidgetMode::PopupMode)
        && inEvent->type() == QEvent::ApplicationStateChange)
    {
        const Qt::ApplicationState changeState = static_cast<QApplicationStateChangeEvent*>(inEvent)->applicationState();
        if (changeState != Qt::ApplicationActive)
        {
            close();
            return true;
        }
    } // 사이즈 조절 가능 모드로 전환
    else if (inObj == _sizeGrip
        && inEvent->type() == QEvent::MouseButtonPress)
    {
        manualSizeMode();
        return false; // no consume
    } // bgframe에 전달된 mouseMove이벤트 후킹
    else if (inObj == ui->bgFrame
        && inEvent->type() == QEvent::MouseMove)
    {
        mouseMoveEvent(static_cast<QMouseEvent*>(inEvent));
        return false; // no consume
    }


    return QWidget::eventFilter(inObj, inEvent);
}

void PopupTranslateWidget::setupUI()
{
    ui->setupUi(this);
    setLayout(ui->outerLayout);

    // ~===========
    // bgFrame shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(ui->bgFrame);
    shadow->setBlurRadius(12);
    shadow->setOffset(0.5);
    shadow->setColor(QColor(0, 0, 0, 255));
    ui->bgFrame->setGraphicsEffect(shadow);

    // ~===========
    // top title layout

    constexpr QSize topButtonsSize{24, 24};
    auto setupTitleWidget = [topButtonsSize, this](QWidget* inWidget, const Qt::Alignment inAlignment)
    {
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(inWidget->sizePolicy().hasHeightForWidth());

        inWidget->setSizePolicy(sizePolicy);
        inWidget->setMinimumSize(topButtonsSize);
        inWidget->setMaximumSize(topButtonsSize);
        inWidget->setFixedSize(topButtonsSize);

        inWidget->setFocusPolicy(Qt::TabFocus);

        ui->titleLayout->addWidget(inWidget, 0, inAlignment);
    };

    // ~===========
    // keepPinButton
    _alwaysOnButton = new SolButton(this);
    _alwaysOnButton->setCheckable(true);
    _alwaysOnButton->setObjectName("alwaysOnButton");
    _alwaysOnButton->setCheckIcon(":/img/keep_pin_fill_v", ":/img/keep_pin_clock45d");
    _alwaysOnButton->setCheckToolTipAction(i18n(Tr::Always_On_Top_Off), i18n(Tr::Always_On_Top_On), Action::PopupAlwaysOn);
    _alwaysOnButton->hide();

    setupTitleWidget(_alwaysOnButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_alwaysOnButton, &QPushButton::toggled, this, &PopupTranslateWidget::onAlwaysOnToggle);

    // ~===========
    // windowModeButton
    _windowModeButton = new SolButton(this);
    _windowModeButton->setCheckable(true);
    _windowModeButton->setObjectName("windowModeButton");
    _windowModeButton->setIcon(QIcon(":/img/window_mode_img"));
    _windowModeButton->setCheckToolTipAction(i18n(Tr::Temp_Window_Mode), i18n(Tr::Normal_Window_Mode), Action::PopupWindowMode);

    setupTitleWidget(_windowModeButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_windowModeButton, &QPushButton::toggled, this, &PopupTranslateWidget::onWindowModeToggle);

    // ~===========
    // _loadingWidget
    _loadingWidget = new LoadingSpinner(":/img/loading_spinner_img", this);
    SolToolTip::setToolTip(_loadingWidget, i18n(Tr::Translating));

    setupTitleWidget(_loadingWidget, Qt::AlignTop | Qt::AlignLeft);

    // ~===========
    // 좌우 버튼 분리
    ui->titleLayout->addStretch(1);

    // ~===========
    // right side

    // ~===========
    // minimize button
    _minimizedButton = new SolButton(this);
    _minimizedButton->setObjectName("minimizedButton");
    _minimizedButton->setIcon(QIcon(":/img/minimize_button_img"));
    _minimizedButton->setToolTipAction(i18n(Tr::Minimize), Action::PopupMinimize);

    setupTitleWidget(_minimizedButton, Qt::AlignTop | Qt::AlignRight);

    connect(_minimizedButton, &QPushButton::clicked, this, &PopupTranslateWidget::onMinimized);

    // ~===========
    // max button
    _maxRestoreButton = new SolButton(this);
    _maxRestoreButton->setCheckable(true);
    _maxRestoreButton->setObjectName("maxRestoreButton");
    _maxRestoreButton->setCheckIcon(":/img/restore_button_img", ":/img/maximize_button_img");
    _maxRestoreButton->setCheckToolTipAction(i18n(Tr::Restore_Previous_Size), i18n(Tr::Maximize), Action::PopupMaxRestore);

    setupTitleWidget(_maxRestoreButton, Qt::AlignTop | Qt::AlignRight);
    connect(_maxRestoreButton, &QPushButton::toggled, this, &PopupTranslateWidget::onMaxNormalToggle);


    // ~===========
    // close button
    _closeButton = new SolButton(this);
    _closeButton->setObjectName("closeButton");
    _closeButton->setIcon(QIcon(":/img/close_button_img"));
    _closeButton->setToolTipAction(i18n(Tr::Close), Action::PopupClose);

    setupTitleWidget(_closeButton, Qt::AlignTop | Qt::AlignRight);

    connect(_closeButton, &QPushButton::clicked, this, &QWidget::close);


    // ~===========
    // bottom statusLayout
    ui->statusLayout->setContentsMargins(5, 0, 5, 5);

    // 복사 버튼
    QPushButton* trCopy = SolWidgetFactory::createCopyButton(this, [this]()
    {
        return (_currentTextCategory == TextCategory::SourceText) ? getSourceText() : getTargetText();
    });
    ui->statusLayout->addWidget(trCopy, 0, Qt::AlignBottom | Qt::AlignLeft);

    // 원문/번역 토글
    _textToggleButton = SolWidgetFactory::createToggleButton(this, [this]() { toggleTranslationText(); });
    _textToggleButton->hide();

    ui->statusLayout->addWidget(_textToggleButton, 0, Qt::AlignBottom | Qt::AlignLeft);


    // 재번역 버튼
    _reTranslateButton = SolWidgetFactory::createReTranslateButton(this, [this]()
    {
        solCore->manager<TranslateManager>()->translateAtPopup(getSourceText(), getTextStyle(), true);
    });
    _reTranslateButton->hide();

    ui->statusLayout->addWidget(_reTranslateButton, 0, Qt::AlignBottom | Qt::AlignLeft);


    // ~=================
    // left/right spacing
    ui->statusLayout->addStretch(1);


    // ~========
    // sizeGrip
    _sizeGrip = new QSizeGrip(this);
    _sizeGrip->show();
    _sizeGrip->installEventFilter(this);

    ui->statusLayout->addWidget(_sizeGrip, 0, Qt::AlignBottom | Qt::AlignRight);


    // ~======================
    // resultText & scroll bar
    ui->textLayout->setContentsMargins(20, 0, 10, 0);

    ui->resultText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Replace the default VScrollBar with an external scrollbar.
    ui->resultText->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    ui->resultText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QSizePolicy scrollSizePolicy = ui->outerVScrollBar->sizePolicy();
    scrollSizePolicy.setRetainSizeWhenHidden(true);
    ui->outerVScrollBar->setSizePolicy(scrollSizePolicy);
    ui->outerVScrollBar->setSingleStep(ui->resultText->verticalScrollBar()->singleStep());

    // Control outer ScrollBar -> inner ScrollBar
    connect(ui->outerVScrollBar, &QScrollBar::valueChanged, this, [this](const int value)
    {
        ui->resultText->verticalScrollBar()->setValue(value);
    });

    // Reflect value inner ScrollBar -> outer ScrollBar
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](int, int)
    {
        syncInOutScrollbar();
    });
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int)
    {
        // SyncInOutScrollbar() calls outer::setValue(), so the outer->inner->outer logic is a loop.
        // However, since setValue() does not emit valueChanged() when it receives the same value,
        // an actual infinite loop does not occur.
        syncInOutScrollbar();
    });
    // Reflect Document information
    connect(ui->resultText->document(), &QTextDocument::contentsChanged, this, [this]()
    {
        syncInOutScrollbar();
    });


    setTabOrder({_windowModeButton, _alwaysOnButton, _minimizedButton, _maxRestoreButton, _closeButton, ui->resultText, _sizeGrip});

    // To ensure that the TabFocus starts in a hidden state.
    _sizeGrip->setFocusPolicy(Qt::TabFocus);
    _sizeGrip->setFocus();
}

void PopupTranslateWidget::executeTranslate(const QString& inSourceText
                                          , const TextStyle inTextStyle
                                          , const LangType inSourceLang
                                          , const LangType inTargetLang
                                          , const bool inIsIgnoreCache)
{
    _loadingWidget->run();

    if ((inTextStyle == TextStyle::PlainText)
        || inIsIgnoreCache)
    {
        executeTranslateImpl(solConfig.currentEngineId()
                           , inSourceText
                           , inTextStyle
                           , inSourceLang
                           , inTargetLang
                           , inIsIgnoreCache);
        return;
    }

    asyncHtmlToMarkdown(inSourceText, this, [this, inSourceLang, inTargetLang](const QString& inMd)
    {
        executeTranslateImpl(solConfig.currentEngineId()
                           , inMd
                           , TextStyle::MarkDown
                           , inSourceLang
                           , inTargetLang
                           , false);
    });
}


void PopupTranslateWidget::completeTranslateText(const QString& inTargetText)
{
    ITranslateWidget::completeTranslateText(inTargetText);

    _loadingWidget->stop();
    _isTranslateComplete = true;
    _textToggleButton->show();
    _reTranslateButton->show();
}

void PopupTranslateWidget::viewTranslationText(const QString& inSourceText
                                             , const QString& inTargetText
                                             , const TextStyle inTextStyle)
{
    setSourceAndStyle(inSourceText, inTextStyle);
    completeTranslateText(inTargetText);
    getVerticalScrollBar()->setValue(0);
}

void PopupTranslateWidget::applyTranslation()
{
    showTranslationPopup();
}

QScrollBar* PopupTranslateWidget::getVerticalScrollBar() const
{
    return ui->resultText->verticalScrollBar();
}

QScrollBar* PopupTranslateWidget::getHorizontalScrollBar() const
{
    return ui->resultText->horizontalScrollBar();
}

QTextCursor PopupTranslateWidget::getTextCursor() const
{
    return ui->resultText->textCursor();
}

void PopupTranslateWidget::setTextCursor(const QTextCursor& inCursor)
{
    ui->resultText->setTextCursor(inCursor);
}


void PopupTranslateWidget::showTranslationPopup()
{
    if ((_prevSize.width() < _maxEditSize.width())
        || (_prevSize.height() < _maxEditSize.height()))
    {
        const QSize newSize = calculateTextEditSize(getTargetText());
        animateTextEditResize(newSize);
    }

    ui->resultText->setFormattingText(getTargetText(), getTextStyle());
}

void PopupTranslateWidget::setTextEditSize(const QSize& inTextEditSize)
{
    // text edit 폭 줄어드는 현상 방지.
    ui->resultText->setFixedSize(inTextEditSize);
    adjustSize();

    // 생성될 스크린 위치 추적
    QScreen* currentScreen = nullptr;
    switch (solConfig.popupScreenPolicy())
    {
    case ScreenPopupPolicy::Default:
    case ScreenPopupPolicy::PrimaryScreen:
        currentScreen = qApp->primaryScreen();
        break;
    case ScreenPopupPolicy::FixedScreen:
        currentScreen = qApp->primaryScreen(); // todo: 추후 저장된 스크린 위치 사용
        break;
    case ScreenPopupPolicy::CursorScreen:
        currentScreen = qApp->screenAt(QCursor::pos());
        break;
    case ScreenPopupPolicy::Size:
        break;
    }

    const QPointF screenTopLeft = currentScreen ? currentScreen->geometry().topLeft() : QPointF();

    // position
    const QSizeF screenSize   = currentScreen ? currentScreen->size().toSizeF() : QSizeF(1920, 1080);
    const QPoint targetCenter = screenTopLeft.toPoint()
            + QPointF(screenSize.width() * _centerPosRatio.x(), screenSize.height() * _centerPosRatio.y()).toPoint();
    const QPoint recCenter = rect().center();

    QPoint targetPos = targetCenter - recCenter;
    targetPos.rx() = qMin(targetPos.x(), static_cast<int>(screenSize.width() - size().width()));
    targetPos.ry() = qMax(targetPos.y(), static_cast<int>(screenSize.height() * _yPosMaxRatio));

    move(targetPos);

    update();
}

QSize PopupTranslateWidget::calculateTextEditSize(const QString& inNewText) const
{
    const auto* textEdit = ui->resultText;

    int newWidth;
    const int newHeight = qBound(_minEditSize.height(), textEdit->document()->size().toSize().height(), _maxEditSize.height());

    if (textEdit->size().width() == _maxEditSize.width())
    {
        newWidth = _maxEditSize.width();
    }
    else
    {
        const QFontMetrics fntMetric = textEdit->fontMetrics();

        const int newStrWidth    = fntMetric.horizontalAdvance(inNewText);
        const int docMarginTwice = static_cast<int>(textEdit->document()->documentMargin() * 2.0);
        const int viewportMargin = textEdit->size().width() - textEdit->viewport()->size().width();
        newWidth                 = qBound(_minEditSize.width(), newStrWidth + viewportMargin + docMarginTwice, _maxEditSize.width());
    }

    return QSize{newWidth, newHeight};
}

void PopupTranslateWidget::animateTextEditResize(const QSize& inNewSize)
{
    if (_prevSize == inNewSize)
    {
        // return;
    }
    _prevSize = inNewSize;

    _animation->stop();
    _animation->setStartValue(ui->resultText->size());
    _animation->setEndValue(inNewSize); // setTextEditSize()
    _animation->start();
}

void PopupTranslateWidget::adjustSizeAfterAnimationFinished()
{
    const int docHeight      = ui->resultText->document()->size().height();
    const int viewportHeight = ui->resultText->viewport()->height();
    if (docHeight > viewportHeight)
    {
        const QSize editSize        = ui->resultText->size();
        const QSize newTextEditSize = QSize{
            qBound(_minEditSize.width(), editSize.width(), _maxEditSize.width())
          , qBound(_minEditSize.height(), qMax(editSize.height(), docHeight), _maxEditSize.height())
        };

        animateTextEditResize(newTextEditSize);
    }
}

void PopupTranslateWidget::calculateTextEditLayoutInfo()
{
    // ~==============================
    // 단계별 마진 및 최소/최대 크기 계산
    if (screen() == nullptr)
    {
        qWarning() << "not detected screen";
    }
    const QSizeF screenSize = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);

    const int minWidth  = screenSize.width() * _minSizeRatio.width();
    const int minHeight = screenSize.height() * _minSizeRatio.height();
    const int maxWidth  = screenSize.width() * _maxSizeRatio.width();
    const int maxHeight = screenSize.height() * _maxSizeRatio.height();

    ui->textLayout->activate();
    const QMargins inMargins = ui->textLayout->contentsMargins()
            + ui->mainLayout->contentsMargins()                         // 메인 컨텐츠 레이아웃 마진
            + QMargins(0, ui->titleLayout->sizeHint().height(), 0, 0)   // 상단 타이틀바 레이아웃 높이
            + QMargins(0, ui->loadingLayout->sizeHint().height(), 0, 0) // 상단 로딩바 레이아웃 높이
            + QMargins(0, 0, 0, ui->statusLayout->sizeHint().height())  // 하단 상태표시 레이아웃 높이
            + QMargins(0, 0, ui->outerVScrollBar->width(), 0);          // 우측 외부 스크롤바 ->sizeHint().width();로 대체 고려해야함

    _outMargins = ui->outerLayout->contentsMargins();

    _innerMarginSize = QSize(inMargins.left() + inMargins.right() + ui->bgFrame->lineWidth()
                           , inMargins.top() + inMargins.bottom() + ui->bgFrame->lineWidth());
    _outerMarginSize = QSize(_outMargins.left() + _outMargins.right(), _outMargins.top() + _outMargins.bottom());


    const QSize totalMarginSize = _innerMarginSize + _outerMarginSize;

    _minEditSize = {minWidth - totalMarginSize.width(), minHeight - totalMarginSize.height()};
    _maxEditSize = {maxWidth - totalMarginSize.width(), maxHeight - totalMarginSize.height()};
}

void PopupTranslateWidget::syncInOutScrollbar()
{
    const QScrollBar* textScroll = ui->resultText->verticalScrollBar();

    const int min = textScroll->minimum();
    const int max = textScroll->maximum();
    const int pageStep = textScroll->pageStep();

    if (min == max)
    {
        ui->outerVScrollBar->hide();
    }
    else
    {
        ui->outerVScrollBar->setRange(min, max);
        ui->outerVScrollBar->setPageStep(pageStep);
        ui->outerVScrollBar->setValue(textScroll->value());
        ui->outerVScrollBar->show();
    }
}

void PopupTranslateWidget::manualSizeMode()
{
    if (_isManualSizeMode)
    {
        return;
    }
    _isManualSizeMode = true;

    // 매뉴얼 사이즈 모드를 위해 등록된 사이즈 그립 이벤트 필터 해제
    _sizeGrip->removeEventFilter(this);

    // ~==================
    // 위젯 사이즈 변경 애니메이션 정지 및 해제
    _animation->stop();
    _animation->setPropertyName("");

    // ~==================
    // 위젯 사이즈 정책 변경
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->bgFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->resultText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const QSize minSize = _minEditSize + _innerMarginSize + _outerMarginSize;
    setMinimumSize(minSize);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->resultText->setMinimumSize(10, 10);
    ui->resultText->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void PopupTranslateWidget::onAlwaysOnToggle(const bool inChecked)
{
    if (_alwaysOnButton->isChecked() != inChecked)
    {
        _alwaysOnButton->setChecked(inChecked);
    }

    manualSizeMode();

#ifdef _WIN32
    const BOOL bIsSet = SetWindowPos(reinterpret_cast<HWND>(winId())
                                   , inChecked ? HWND_TOPMOST : HWND_NOTOPMOST
                                   , 0, 0, 0, 0
                                   , SWP_NOMOVE | SWP_NOSIZE);
    if (bIsSet == false)
    {
        solDebug << "AlwaysOn" << (inChecked ? "Top" : "NoTop") << "set failed";
    }
#else
    if (windowFlags().testFlag(Qt::WindowStaysOnTopHint) != inChecked)
    {
        setWindowFlag(Qt::WindowStaysOnTopHint, inChecked);
        show();
    }
#endif
}

void PopupTranslateWidget::onWindowModeToggle(const bool inChecked)
{
    if (inChecked)
    {
        changeNormalWindowMode();
    }
    else
    {
        changePopupMode();
    }
}

void PopupTranslateWidget::changeNormalWindowMode()
{
    // 자동닫기 해제
    qApp->removeEventFilter(this);

    // 처음부터 일반모드로 시작하는 경우 매뉴얼 모드로 변경하지 않습니다.
    if (_widgetModeFlags.testFlag(SolWidgetMode::PopupMode))
    {
        manualSizeMode();
    }
    _widgetModeFlags.setFlag(SolWidgetMode::PopupMode, false);

    const bool hasWModeBtnFocus = _windowModeButton->hasFocus();
    _windowModeButton->hide();
    if (_alwaysOnButton->isHidden())
    {
        _alwaysOnButton->show();
    }

    // 대체되는 버튼에 포커스 이동.
    if (hasWModeBtnFocus)
    {
        _alwaysOnButton->setFocus(Qt::TabFocusReason);
    }
}

void PopupTranslateWidget::changePopupMode()
{
    // 팝업모드에서 자동 닫기 기능 등록
    qApp->installEventFilter(this);

    _widgetModeFlags.setFlag(SolWidgetMode::PopupMode);

    if (_alwaysOnButton->isHidden() == false)
    {
        _alwaysOnButton->hide();
    }
}

void PopupTranslateWidget::setMaxNormal(const bool inIsMaximize)
{
    _maxRestoreButton->setChecked(inIsMaximize);
}

void PopupTranslateWidget::onMaxNormalToggle(const bool inIsMaximize)
{
    if (_isMaximizedMode == inIsMaximize)
    {
        return;
    }

    if (inIsMaximize)
    {
        manualSizeMode();
        changeNormalWindowMode();
        if (_isMaximizedMode == false)
        {
            showMaximized();
        }
    }
    else
    {
        if (_isMaximizedMode)
        {
            showNormal();
        }
    }

    _isMaximizedMode = inIsMaximize;
}

void PopupTranslateWidget::onMinimized()
{
    manualSizeMode();
    changeNormalWindowMode();
    showMinimized();
}

void PopupTranslateWidget::toggleTranslationText()
{
    if (_isTranslateComplete == false)
    {
        return;
    }

    // fix scrollbar
    const int prevVerticalScrollVal = ui->resultText->verticalScrollBar()->value();

    // toggle
    _currentTextCategory = (_currentTextCategory == TextCategory::SourceText) ? TextCategory::TargetText : TextCategory::SourceText;

    const QString nextText = (_currentTextCategory == TextCategory::SourceText) ? getSourceText() : getTargetText();

    ui->resultText->setFormattingText(nextText, getTextStyle());

    // fix scrollbar
    ui->resultText->verticalScrollBar()->setValue(prevVerticalScrollVal);
}

void PopupTranslateWidget::setShadowEffectEnabled(const bool inIsEnable)
{
    ui->bgFrame->graphicsEffect()->setEnabled(inIsEnable);
}

void PopupTranslateWidget::detectFocusInOut(const QWidget* inOld, const QWidget* inNow)
{
    if (isThis(this, inOld))
    {
        if (isThis(this, inNow))
        {
            return;
        }
        else
        {
            setShadowEffectEnabled(false);
            return;
        }
    }
    if (isThis(this, inNow))
    {
        setShadowEffectEnabled(true);
    }
}

QRect PopupTranslateWidget::getInnerGeometry() const
{
    constexpr int margin       = 15;
    constexpr QMargins margins = {margin, margin, margin, margin};
    return frameGeometry() - margins;
}

void PopupTranslateWidget::moveWindow(const QPoint& inMousePos)
{
    if (_isMaximizedMode)
    {
        // 내부 QFrame의 절대 좌표와, QFrame 기준 상대 좌표 계산
        const QPoint outMarginTopLeft = QPoint(_outMargins.left(), _outMargins.top());
        const QPoint frameTopLeft     = frameGeometry().topLeft() + outMarginTopLeft;
        const QPointF mouseRelPointF  = (inMousePos - frameTopLeft).toPointF();

        // frame 기준 사이즈
        const QSizeF maxFrameSizeF   = frameGeometry().size().toSizeF() - _outerMarginSize;
        const QSizeF normalSizeF     = normalGeometry().size().toSizeF() - _outerMarginSize;
        const QSizeF halfNormalSizeF = normalSizeF / 2.0;

        const qreal leftInterval   = mouseRelPointF.x();
        const qreal rightInterval  = maxFrameSizeF.width() - mouseRelPointF.x();
        const qreal topInterval    = mouseRelPointF.y();
        const qreal bottomInterval = maxFrameSizeF.height() - mouseRelPointF.y();

        QPoint normalRelMousePoint;
        if (leftInterval <= halfNormalSizeF.width())
        {
            normalRelMousePoint.rx() = leftInterval;
        }
        else if (rightInterval <= halfNormalSizeF.width())
        {
            normalRelMousePoint.rx() = normalSizeF.width() - rightInterval;
        }
        else
        {
            normalRelMousePoint.rx() = halfNormalSizeF.width();
        }

        if (topInterval <= halfNormalSizeF.height())
        {
            normalRelMousePoint.ry() = topInterval;
        }
        else if (bottomInterval <= halfNormalSizeF.height())
        {
            normalRelMousePoint.ry() = normalSizeF.height() - bottomInterval;
        }
        else
        {
            normalRelMousePoint.ry() = halfNormalSizeF.height();
        }


        const QPoint newNormalWindowPos = inMousePos - normalRelMousePoint - outMarginTopLeft;
        setMaxNormal(false);
        move(newNormalWindowPos);
        _dragPoint = inMousePos - newNormalWindowPos;
    }
    else if (getInnerGeometry().contains(inMousePos))
    {
        if (QWindow* win = windowHandle())
        {
            const bool isSupportSystemMove = win->startSystemMove();
            if (isSupportSystemMove == false)
            {
                move(inMousePos - _dragPoint);
            }
        }
    }
}

namespace
{
enum class GeoArea
{
    topLeft, topRight, bottomLeft, bottomRight, top, bottom, Left, Right, None
};

GeoArea containsPosInSpanArea(QRect inGeo
                            , QRect inInGeo
                            , const QPoint& inPos)
{
    inGeo.setBottomRight(inGeo.bottomRight() + QPoint{1, 1});
    inInGeo.setBottomRight(inInGeo.bottomRight() + QPoint{1, 1});

    if (QRect::span(inGeo.topLeft(), inInGeo.topLeft()).contains(inPos))         return GeoArea::topLeft;
    if (QRect::span(inGeo.topRight(), inInGeo.topRight()).contains(inPos))       return GeoArea::topRight;
    if (QRect::span(inGeo.bottomLeft(), inInGeo.bottomLeft()).contains(inPos))   return GeoArea::bottomLeft;
    if (QRect::span(inGeo.bottomRight(), inInGeo.bottomRight()).contains(inPos)) return GeoArea::bottomRight;
    if (QRect::span(inGeo.topLeft(), inInGeo.topRight()).contains(inPos))        return GeoArea::top;
    if (QRect::span(inGeo.bottomLeft(), inInGeo.bottomRight()).contains(inPos))  return GeoArea::bottom;
    if (QRect::span(inGeo.topLeft(), inInGeo.bottomLeft()).contains(inPos))      return GeoArea::Left;
    if (QRect::span(inGeo.topRight(), inInGeo.bottomRight()).contains(inPos))    return GeoArea::Right;

    return GeoArea::None;
}
} // anonymous namespace

void PopupTranslateWidget::resizeWindow(const QPoint& inMousePos)
{
    manualSizeMode();

    QWindow* win = windowHandle();

    const QRect geo    = frameGeometry();
    const QRect innGeo = getInnerGeometry();

    if (win == nullptr || innGeo.contains(inMousePos))
    {
        return;
    }

    switch (containsPosInSpanArea(geo, innGeo, inMousePos))
    {
    case GeoArea::topLeft:     win->startSystemResize(Qt::TopEdge | Qt::LeftEdge); break;
    case GeoArea::topRight:    win->startSystemResize(Qt::TopEdge | Qt::RightEdge); break;
    case GeoArea::bottomLeft:  win->startSystemResize(Qt::BottomEdge | Qt::LeftEdge); break;
    case GeoArea::bottomRight: win->startSystemResize(Qt::BottomEdge | Qt::RightEdge); break;
    case GeoArea::top:         win->startSystemResize(Qt::TopEdge); break;
    case GeoArea::bottom:      win->startSystemResize(Qt::BottomEdge); break;
    case GeoArea::Left:        win->startSystemResize(Qt::LeftEdge); break;
    case GeoArea::Right:       win->startSystemResize(Qt::RightEdge); break;
    case GeoArea::None:        break;
    }
}

void PopupTranslateWidget::setCursorShape(const QPoint& inMousePos)
{
    const QRect geo    = frameGeometry();
    const QRect innGeo = getInnerGeometry();

    Qt::CursorShape cursorShape = Qt::ArrowCursor;

    switch (containsPosInSpanArea(geo, innGeo, inMousePos))
    {
    case GeoArea::topLeft:     cursorShape = Qt::SizeFDiagCursor; break;
    case GeoArea::topRight:    cursorShape = Qt::SizeBDiagCursor; break;
    case GeoArea::bottomLeft:  cursorShape = Qt::SizeBDiagCursor; break;
    case GeoArea::bottomRight: cursorShape = Qt::SizeFDiagCursor; break;
    case GeoArea::top:         cursorShape = Qt::SizeVerCursor; break;
    case GeoArea::bottom:      cursorShape = Qt::SizeVerCursor; break;
    case GeoArea::Left:        cursorShape = Qt::SizeHorCursor; break;
    case GeoArea::Right:       cursorShape = Qt::SizeHorCursor; break;
    case GeoArea::None:        cursorShape = Qt::ArrowCursor; break;
    }

    setCursor(cursorShape);
}

void PopupTranslateWidget::mousePressEvent(QMouseEvent* inEvent)
{
    if (inEvent->button() == Qt::LeftButton)
    {
        _dragPoint = inEvent->globalPosition().toPoint() - frameGeometry().topLeft();

        _isDrag = true;
        if (_isMaximizedMode == false)
        {
            resizeWindow(inEvent->globalPosition().toPoint());
        }

        inEvent->accept();
    }
}

void PopupTranslateWidget::mouseDoubleClickEvent(QMouseEvent* inEvent)
{
    if (inEvent->button() == Qt::LeftButton)
    {
        setMaxNormal(!_isMaximizedMode);

        inEvent->accept();
    }

    QWidget::mouseDoubleClickEvent(inEvent);
}

void PopupTranslateWidget::mouseMoveEvent(QMouseEvent* inEvent)
{
    const QPoint eventPoint = inEvent->globalPosition().toPoint();

    if (_isMaximizedMode == false)
    {
        setCursorShape(eventPoint);
    }

    if (_isDrag == false)
    {
        return;
    }

    manualSizeMode();

    moveWindow(eventPoint);
    inEvent->accept();
}

void PopupTranslateWidget::mouseReleaseEvent(QMouseEvent* inEvent)
{
    if (inEvent->button() == Qt::LeftButton)
    {
        _isDrag = false;
        inEvent->accept();
    }
}

void PopupTranslateWidget::enterEvent(QEnterEvent* inEvent)
{
    setShadowEffectEnabled(true);

    QWidget::enterEvent(inEvent);
}

void PopupTranslateWidget::leaveEvent(QEvent* inEvent)
{
    bool hasChildFocus = hasFocus();

    const QList<QWidget*> childList = findChildren<QWidget*>();
    for (const QWidget* childWidget : childList)
    {
        if (hasChildFocus)
        {
            break;
        }
        if (childWidget->hasFocus())
        {
            hasChildFocus = true;
        }
    }

    if (hasChildFocus == false)
    {
        setShadowEffectEnabled(false);
    }
    setCursor(Qt::ArrowCursor);

    QWidget::leaveEvent(inEvent);
}
} // namespace Sol
