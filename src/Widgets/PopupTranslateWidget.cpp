// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "PopupTranslateWidget.h"

#ifdef _WIN32
#include <qt_windows.h>
#endif

#include <QAbstractTextDocumentLayout>
#include <QBoxLayout>
#include <QClipboard>
#include <QFuturewatcher>
#include <QGraphicsDropShadowEffect>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QShortcut>
#include <QSizeGrip>
#include <QSvgWidget>
#include <QtConcurrentRun>
#include <QTimer>
#include <QTextBlock>
#include <QWindow>

#include <qevent.h>

#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolUtilibrary.h"

#include "Managers/AsyncManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/SolToast.h"
#include "SubWidgets/SolToolTip.h"
#include "SubWidgets/LoadingBar.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolWidgetFactory.h"

#include "Widgets/ui_PopupTranslateWidget.h"

PopupTranslateWidget::PopupTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , _minSizeRatio(0.15, 0.15)
    , _maxSizeRatio(0.2, 0.65)
    , _fullSizeRatio(0.95, 0.95)
    , _centerPosRatio(0.85, 0.33)
    , _yPosMaxRatio((1.0 - _maxSizeRatio.height()) / 2.0)
    , ui(new Ui::PopupTranslateWidget)
{
    QIcon icon = QIcon(":/img/icon_img");
    setWindowIcon(icon);
    setWindowTitle(tr("sol"));

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

    if (solConfig.getIsPopupTrWindowTemp())
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

void PopupTranslateWidget::executeTranslateImpl(const QString& inOriginText
                                              , const TextStyle inTextStyle
                                              , const LangType inSourceLang
                                              , const LangType inTargetLang)
{
    _originText = inOriginText;
    _textStyle  = inTextStyle;
    solCore->translateManager()->translateText(TranslateRequestInfo{
        this
      , false
      , solConfig.getCurrentEngineType()
      , inOriginText
      , inTextStyle
      , inSourceLang
      , inTargetLang
      , this
      , [this, inTextStyle](const QString& inStr) { completeTransText(inStr, inTextStyle); }
      , this
      , [this, inTextStyle](const QString& inStr) { streamTransText(inStr, inTextStyle); }
    });
}

void PopupTranslateWidget::executeTranslate(const QString& inOriginText
                                          , const TextStyle inTextStyle
                                          , const LangType inSourceLang
                                          , const LangType inTargetLang)
{
    if (inTextStyle == TextStyle::PlainText)
    {
        executeTranslateImpl(inOriginText, TextStyle::PlainText, inSourceLang, inTargetLang);
        return;
    }

    AsyncManager::asyncLaunch<QString>(
        this,
        [htmlStr = std::move(inOriginText)]() mutable
        {
            // list 무시하는 문법 제거.
            QTextDocument txtDoc;
            txtDoc.setHtml(htmlStr.replace(QRegularExpression(R"(list-style: none)"), ""));

            return txtDoc.toMarkdown();
        },
        [this, inSourceLang, inTargetLang](const QString& inMd)
        {
            executeTranslateImpl(inMd, TextStyle::PlainText, inSourceLang, inTargetLang);
        });
}


void PopupTranslateWidget::completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    ITranslateWidget::completeTransText(inTranslatedText, inTextStyle);

    _loadingBar->stop();
    _isTranslateComplete = true;
    _textToggleButton->show();
}

void PopupTranslateWidget::applyTranslation()
{
    showTranslationPopup();
}

QScrollBar* PopupTranslateWidget::getVerticalScrollBar()
{
    return ui->resultText->verticalScrollBar();
}

QScrollBar* PopupTranslateWidget::getHorizontalScrollBar()
{
    return ui->resultText->horizontalScrollBar();
}

QTextCursor PopupTranslateWidget::getTextCursor()
{
    return ui->resultText->textCursor();
}

void PopupTranslateWidget::setTextCursor(const QTextCursor& cursor)
{
    ui->resultText->setTextCursor(cursor);
}


void PopupTranslateWidget::showTranslationPopup()
{
    if ((_prevSize.width() < _maxEditSize.width())
        || (_prevSize.height() < _maxEditSize.height()))
    {
        const QSize newSize = calculateTextEditSize(getTranslatedText());
        animateTextEditResize(newSize);
    }

    ui->resultText->setFormattingText(getTranslatedText(), getTranslatedTextStyle());
}

void PopupTranslateWidget::setTextEditSize(const QSize& inTextEditSize)
{
    // text edit 폭 줄어드는 현상 방지.
    ui->resultText->setFixedSize(inTextEditSize);
    adjustSize();

    // 생성될 스크린 위치 추적
    QScreen* currentScreen = nullptr;
    switch (solConfig.getSimplePopupScreenPolicy())
    {
    case sol::ScreenPopupPolicy::Default:
    case sol::ScreenPopupPolicy::PrimaryScreen:
        currentScreen = qApp->primaryScreen();
        break;
    case sol::ScreenPopupPolicy::FixedScreen:
        currentScreen = qApp->primaryScreen(); // 임시. 추후 저장된 스크린 위치 사용
        break;
    case sol::ScreenPopupPolicy::CursorScreen:
        currentScreen = qApp->screenAt(QCursor::pos());
        break;
    case sol::ScreenPopupPolicy::Size:
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

void PopupTranslateWidget::manualSizeMode()
{
    if (_bManualSizeMode)
    {
        return;
    }
    _bManualSizeMode = true;

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
    auto getTitleLastColumn = [this]() { return ui->titleLayout->columnCount(); };
    auto setupTitleButton   = [=, this](QPushButton* inButton, const Qt::Alignment inAlignment)
    {
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(inButton->sizePolicy().hasHeightForWidth());
        inButton->setSizePolicy(sizePolicy);
        inButton->setMinimumSize(topButtonsSize);
        inButton->setMaximumSize(topButtonsSize);
        inButton->setFixedSize(topButtonsSize);

        inButton->setFocusPolicy(Qt::TabFocus);
        inButton->setFlat(true);

        ui->titleLayout->addWidget(inButton, 0, getTitleLastColumn(), inAlignment);
    };

    // ~===========
    // keepPinButton
    _AlwaysOnButton = new SolButton(this);
    _AlwaysOnButton->setCheckable(true);
    _AlwaysOnButton->setObjectName("alwaysOnButton");
    _AlwaysOnButton->setCheckIcon(":/img/keep_pin_fill_v", ":/img/keep_pin_clock45d");
    _AlwaysOnButton->setCheckToolTipShortcut(tr("항상 위 끄기"), tr("항상 위 켜기"), Qt::Key_A);
    _AlwaysOnButton->hide();

    setupTitleButton(_AlwaysOnButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_AlwaysOnButton, &QPushButton::toggled, this, &PopupTranslateWidget::onAlwaysOnToggle);

    // ~===========
    // windowModeButton
    _windowModeButton = new SolButton(this);
    _windowModeButton->setCheckable(true);
    _windowModeButton->setObjectName("windowModeButton");
    _windowModeButton->setIcon(QIcon(":/img/window_mode_img"));
    _windowModeButton->setCheckToolTipShortcut(tr("임시창 모드"), tr("일반창 모드"), Qt::Key_N);

    setupTitleButton(_windowModeButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_windowModeButton, &QPushButton::toggled, this, &PopupTranslateWidget::onWindowModeToggle);


    // 좌우 버튼 분리
    ui->titleLayout->setColumnStretch(getTitleLastColumn() - 1, 1);

    // ~===========
    // right side

    // ~===========
    // minimize button
    _minimizedButton = new SolButton(this);
    _minimizedButton->setObjectName("minimizedButton");
    _minimizedButton->setIcon(QIcon(":/img/minimize_button_img"));
    _minimizedButton->setToolTipShortcut(tr("최소화"), Qt::Key_M);

    setupTitleButton(_minimizedButton, Qt::AlignTop | Qt::AlignRight);

    connect(_minimizedButton, &QPushButton::clicked, this, &PopupTranslateWidget::onMinimized);

    // ~===========
    // max button
    _maxRestoreButton = new SolButton(this);
    _maxRestoreButton->setCheckable(true);
    _maxRestoreButton->setObjectName("maxRestoreButton");
    _maxRestoreButton->setCheckIcon(":/img/restore_button_img", ":/img/maximize_button_img");
    _maxRestoreButton->setCheckToolTipShortcut(tr("이전 크기로 복원"), tr("최대화"), Qt::Key_G);

    setupTitleButton(_maxRestoreButton, Qt::AlignTop | Qt::AlignRight);
    connect(_maxRestoreButton, &QPushButton::toggled, this, &PopupTranslateWidget::onMaxNormalToggle);


    // ~===========
    // close button
    _closeButton = new SolButton(this);
    _closeButton->setObjectName("closeButton");
    _closeButton->setIcon(QIcon(":/img/close_button_img"));
    _closeButton->setToolTipShortcut(tr("닫기"), Qt::Key_Escape);

    setupTitleButton(_closeButton, Qt::AlignTop | Qt::AlignRight);

    connect(_closeButton, &QPushButton::clicked, this, &QWidget::close);


    // ~===========
    // bottom statusLayout
    ui->statusLayout->setContentsMargins(5, 0, 5, 5);

    // 복사 버튼
    QPushButton* trCopy = SolWidgetFactory::createCopyButton(this, [this]()
    {
        if (_currentTextType == TextType::OriginText)
        {
            return _originText;
        }
        else
        {
            return getTranslatedText();
        }
    });

    ui->statusLayout->addWidget(trCopy, 0, Qt::AlignBottom | Qt::AlignLeft);

    // 원문/번역 토글
    _textToggleButton = new SolButton(this);
    _textToggleButton->setIcon(QIcon(":/img/swap_text_img"));
    _textToggleButton->setFocusPolicy(Qt::TabFocus);
    _textToggleButton->setToolTipShortcut(tr("원문/번역 토글"), Qt::Key_T);

    ui->statusLayout->addWidget(_textToggleButton, 0, Qt::AlignBottom | Qt::AlignLeft);

    connect(_textToggleButton, &QPushButton::clicked, this, &PopupTranslateWidget::toggleTranslationText);
    _textToggleButton->hide();

    ui->statusLayout->addStretch(1);

    // sizeGrip
    _sizeGrip = new QSizeGrip(this);
    _sizeGrip->show();
    _sizeGrip->installEventFilter(this);

    ui->statusLayout->addWidget(_sizeGrip, 0, Qt::AlignBottom | Qt::AlignRight);


    // ~===========
    // _loadingBar
    _loadingBar = new LoadingBar(":/img/wait_anim_img", this);
    ui->loadingLayout->addWidget(_loadingBar, 0, 0);
    _loadingBar->run();

    // ~======================
    // resultText & scroll bar
    ui->textLayout->setContentsMargins(20, 0, 10, 0);

    ui->resultText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 기본 수직 스크롤바를 외부 스크롤바로 대체
    ui->resultText->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    ui->resultText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QSizePolicy scrollSizePolicy = ui->outerVScrollBar->sizePolicy();
    scrollSizePolicy.setRetainSizeWhenHidden(true);
    ui->outerVScrollBar->setSizePolicy(scrollSizePolicy);

    // 외부 스크롤바 -> 내부 스크롤바 제어
    connect(ui->outerVScrollBar, &QScrollBar::valueChanged, this, [this](const int value)
    {
        ui->resultText->verticalScrollBar()->setValue(value);
    });

    // 내부 스크롤바 값 -> 외부 스크롤바에 반영
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](int, int)
    {
        syncInOutScrollbar();
    });
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int)
    {
        syncInOutScrollbar();
    });
    // 문서 정보 반영
    connect(ui->resultText->document(), &QTextDocument::contentsChanged, this, [this]()
    {
        syncInOutScrollbar();
    });


    setTabOrder({_windowModeButton, _AlwaysOnButton, _minimizedButton, _maxRestoreButton, _closeButton, ui->resultText, _sizeGrip});
    // 탭 포커스가 안보이는 상태로 시작할 수 있도록 하기 위함.
    _sizeGrip->setFocusPolicy(Qt::TabFocus);
    _sizeGrip->setFocus();
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
    const QSizeF screenSize  = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);

    const int minWidth  = screenSize.width() * _minSizeRatio.width();
    const int minHeight = screenSize.height() * _minSizeRatio.height();
    const int maxWidth  = screenSize.width() * _maxSizeRatio.width();
    const int maxHeight = screenSize.height() * _maxSizeRatio.height();

    ui->textLayout->activate();
    const QMargins inMargins = ui->textLayout->contentsMargins()
            + ui->mainLayout->contentsMargins()                        // 메인 컨텐츠 레이아웃 마진
            + QMargins(0, ui->titleLayout->sizeHint().height(), 0, 0)  // 상단 타이틀바 레이아웃 높이
            + QMargins(0, ui->loadingLayout->sizeHint().height(), 0, 0)// 상단 로딩바 레이아웃 높이
            + QMargins(0, 0, 0, ui->statusLayout->sizeHint().height()) // 하단 상태표시 레이아웃 높이
            + QMargins(0, 0, ui->outerVScrollBar->width(), 0);         // 우측 외부 스크롤바 ->sizeHint().width();로 대체 고려해야함

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

void PopupTranslateWidget::onAlwaysOnToggle(bool checked)
{
    if (_AlwaysOnButton->isChecked() != checked)
    {
        _AlwaysOnButton->setChecked(checked);
    }

    manualSizeMode();

#ifdef _WIN32
    BOOL bIsSet = SetWindowPos(reinterpret_cast<HWND>(winId())
                             , checked ? HWND_TOPMOST : HWND_NOTOPMOST
                             , 0, 0, 0, 0
                             , SWP_NOMOVE | SWP_NOSIZE);
    if (bIsSet == false)
    {
        solDebug << "AlwaysOn" << (checked ? "Top" : "NoTop") << "set failed";
    }
#else
    if (windowFlags().testFlag(Qt::WindowStaysOnTopHint) != checked)
    {
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        show();
    }
#endif
}

void PopupTranslateWidget::onWindowModeToggle(bool checked)
{
    if (checked)
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

    const bool bHasWModeBtnFocus = _windowModeButton->hasFocus();
    _windowModeButton->hide();
    if (_AlwaysOnButton->isHidden())
    {
        _AlwaysOnButton->show();
    }

    // 대체되는 버튼에 포커스 이동.
    if (bHasWModeBtnFocus)
    {
        _AlwaysOnButton->setFocus(Qt::TabFocusReason);
    }
}

void PopupTranslateWidget::changePopupMode()
{
    // 팝업모드에서 자동 닫기 기능 등록
    qApp->installEventFilter(this);

    _widgetModeFlags.setFlag(SolWidgetMode::PopupMode);

    if (_AlwaysOnButton->isHidden() == false)
    {
        _AlwaysOnButton->hide();
    }
}

void PopupTranslateWidget::setMaxNormal(const bool bMaximize)
{
    _maxRestoreButton->setChecked(bMaximize);
}

void PopupTranslateWidget::onMaxNormalToggle(const bool bMaximize)
{
    if (_bMaximizedMode == bMaximize)
    {
        return;
    }

    if (bMaximize)
    {
        manualSizeMode();
        changeNormalWindowMode();
        if (_bMaximizedMode == false)
        {
            showMaximized();
        }
    }
    else
    {
        if (_bMaximizedMode)
        {
            showNormal();
        }
    }
    _bMaximizedMode = bMaximize;
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
    QString nextText;
    if (_currentTextType == TextType::OriginText)
    {
        _currentTextType = TextType::TranslateText;
        nextText = getTranslatedText();
    }
    else
    {
        _currentTextType = TextType::OriginText;
        nextText = _originText;
    }
    ui->resultText->setFormattingText(nextText, _textStyle);

    // fix scrollbar
    ui->resultText->verticalScrollBar()->setValue(prevVerticalScrollVal);
}

void PopupTranslateWidget::setShadowEffectEnabled(const bool bIsEnable)
{
    ui->bgFrame->graphicsEffect()->setEnabled(bIsEnable);
}

void PopupTranslateWidget::detectFocusInOut(QWidget* old, QWidget* now)
{
    if (sol::isThis(this, old))
    {
        if (sol::isThis(this, now))
        {
            return;
        }
        else
        {
            setShadowEffectEnabled(false);
            return;
        }
    }
    if (sol::isThis(this, now))
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
    if (_bMaximizedMode)
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
            const bool bSystemMove = win->startSystemMove();
            if (bSystemMove == false)
            {
                move(inMousePos - _dragPoint);
            }
        }
    }
}

void PopupTranslateWidget::resizeWindow(const QPoint& inMousePos)
{
    manualSizeMode();
    if (QWindow* win = windowHandle())
    {
        const QRect geo    = frameGeometry();
        const QRect innGeo = getInnerGeometry();

        if (innGeo.contains(inMousePos))
            return;

        const QRect topLeftArea     = QRect::span(geo.topLeft(), innGeo.topLeft());
        const QRect topRightArea    = QRect::span(geo.topRight(), innGeo.topRight());
        const QRect bottomLeftArea  = QRect::span(geo.bottomLeft(), innGeo.bottomLeft());
        const QRect bottomRightArea = QRect::span(geo.bottomRight(), innGeo.bottomRight());
        const QRect topArea         = QRect::span(geo.topLeft(), innGeo.topRight());
        const QRect bottomArea      = QRect::span(geo.bottomLeft(), innGeo.bottomRight());
        const QRect LeftArea        = QRect::span(geo.topLeft(), innGeo.bottomLeft());
        const QRect RightArea       = QRect::span(geo.topRight(), innGeo.bottomRight());

        if (topLeftArea.contains(inMousePos))          win->startSystemResize(Qt::TopEdge | Qt::LeftEdge);
        else if (topRightArea.contains(inMousePos))    win->startSystemResize(Qt::TopEdge | Qt::RightEdge);
        else if (bottomLeftArea.contains(inMousePos))  win->startSystemResize(Qt::BottomEdge | Qt::LeftEdge);
        else if (bottomRightArea.contains(inMousePos)) win->startSystemResize(Qt::BottomEdge | Qt::RightEdge);
        else if (topArea.contains(inMousePos))         win->startSystemResize(Qt::TopEdge);
        else if (bottomArea.contains(inMousePos))      win->startSystemResize(Qt::BottomEdge);
        else if (LeftArea.contains(inMousePos))        win->startSystemResize(Qt::LeftEdge);
        else if (RightArea.contains(inMousePos))       win->startSystemResize(Qt::RightEdge);
    }
}

void PopupTranslateWidget::setCursorShape(const QPoint& inMousePos)
{
    const QRect geo = frameGeometry();
    const QRect innGeo = getInnerGeometry();

    const QRect topLeftArea     = QRect::span(geo.topLeft(), innGeo.topLeft());
    const QRect topRightArea    = QRect::span(geo.topRight(), innGeo.topRight());
    const QRect bottomLeftArea  = QRect::span(geo.bottomLeft(), innGeo.bottomLeft());
    const QRect bottomRightArea = QRect::span(geo.bottomRight(), innGeo.bottomRight());
    const QRect topArea         = QRect::span(geo.topLeft(), innGeo.topRight());
    const QRect bottomArea      = QRect::span(geo.bottomLeft(), innGeo.bottomRight());
    const QRect LeftArea        = QRect::span(geo.topLeft(), innGeo.bottomLeft());
    const QRect RightArea       = QRect::span(geo.topRight(), innGeo.bottomRight());

    Qt::CursorShape cursorShape = Qt::ArrowCursor;

    if (innGeo.contains(inMousePos))               cursorShape = Qt::ArrowCursor;
    else if (topLeftArea.contains(inMousePos))     cursorShape = Qt::SizeFDiagCursor;
    else if (topRightArea.contains(inMousePos))    cursorShape = Qt::SizeBDiagCursor;
    else if (bottomLeftArea.contains(inMousePos))  cursorShape = Qt::SizeBDiagCursor;
    else if (bottomRightArea.contains(inMousePos)) cursorShape = Qt::SizeFDiagCursor;
    else if (topArea.contains(inMousePos))         cursorShape = Qt::SizeVerCursor;
    else if (bottomArea.contains(inMousePos))      cursorShape = Qt::SizeVerCursor;
    else if (LeftArea.contains(inMousePos))        cursorShape = Qt::SizeHorCursor;
    else if (RightArea.contains(inMousePos))       cursorShape = Qt::SizeHorCursor;

    setCursor(cursorShape);
}

void PopupTranslateWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _dragPoint = event->globalPosition().toPoint() - frameGeometry().topLeft();

        _bIsDrag = true;
        if (_bMaximizedMode == false)
        {
            resizeWindow(event->globalPosition().toPoint());
        }

        event->accept();
    }
}

void PopupTranslateWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        setMaxNormal(!_bMaximizedMode);

        event->accept();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void PopupTranslateWidget::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint eventPoint = event->globalPosition().toPoint();

    if (_bMaximizedMode == false)
    {
        setCursorShape(eventPoint);
    }

    if (_bIsDrag == false)
    {
        return;
    }

    manualSizeMode();

    moveWindow(eventPoint);
    event->accept();
}

void PopupTranslateWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _bIsDrag = false;
        event->accept();
    }
}

void PopupTranslateWidget::enterEvent(QEnterEvent* event)
{
    setShadowEffectEnabled(true);

    QWidget::enterEvent(event);
}

void PopupTranslateWidget::leaveEvent(QEvent* event)
{
    bool hasChildFocus = hasFocus();

    const QList<QWidget*> childList = findChildren<QWidget*>();
    for (QWidget* childWidget : childList)
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

    QWidget::leaveEvent(event);
}

bool PopupTranslateWidget::eventFilter(QObject* obj, QEvent* event)
{
    // 팝업모드에서 자동 종료
    if (obj == qApp
        && _widgetModeFlags.testFlag(SolWidgetMode::PopupMode)
        && event->type() == QEvent::ApplicationStateChange)
    {
        Qt::ApplicationState changeState = static_cast<QApplicationStateChangeEvent*>(event)->applicationState();
        if (changeState != Qt::ApplicationActive)
        {
            close();
            return true;
        }
    } // 사이즈 조절 가능 모드로 전환
    else if (obj == _sizeGrip
        && event->type() == QEvent::MouseButtonPress)
    {
        manualSizeMode();
        return false; // no consume
    } // bgframe에 전달된 mouseMove이벤트 후킹
    else if (obj == ui->bgFrame
        && event->type() == QEvent::MouseMove)
    {
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
        return false; // no consume
    }


    return QWidget::eventFilter(obj, event);
}
