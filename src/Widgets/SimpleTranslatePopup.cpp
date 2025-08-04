// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SimpleTranslatePopup.h"

#ifdef _WIN32
#include <qt_windows.h>
#endif

#include <QAbstractTextDocumentLayout>
#include <QBoxLayout>
#include <QFuturewatcher>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QSizeGrip>
#include <QSvgWidget>
#include <QtConcurrentRun>
#include <QTimer>
#include <QTextBlock>
#include <qevent.h>

#include "FinUtilibrary.h"

#include "Managers/ConfigManager.h"

#include "SubWidgets/FinToolTip.h"
#include "SubWidgets/LoadingBar.h"

#include "Widgets/ui_SimpleTranslatePopup.h"

constexpr QColor codeBgColor(29, 29, 29, 255);

const QString codeBgColorStr = QString::fromLatin1("rgba(%1,%2,%3,%4)")
                               .arg(codeBgColor.red())
                               .arg(codeBgColor.green())
                               .arg(codeBgColor.blue())
                               .arg(codeBgColor.alpha());

SimpleTranslatePopup::SimpleTranslatePopup(QWidget* parent)
    : ITranslateWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , ui(new Ui::SimpleTranslatePopup)
{
    QIcon icon = QIcon(":/img/icon_img");
    setWindowIcon(icon);
    setWindowTitle(tr("fin"));

    // ~===========
    // config
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    // ~===========
    // ui
    setupUI();

    // 포커스 변경에 따른 그림자 on/off 제어. (그림자 성능)
    connect(qApp, &QApplication::focusChanged, this, &SimpleTranslatePopup::detectFocusInOut);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->bgFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->resultText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    changePopupMode();

    // ~======================
    // 애니메이션
    _animation = new QPropertyAnimation(this, "textEditSize", this); // setTextEditSize 함수 연결
    _animation->setDuration(250);
    _animation->setEasingCurve(QEasingCurve::OutQuad);
    connect(_animation, &QAbstractAnimation::finished, this, &SimpleTranslatePopup::adjustSizeAfterAnimationFinished);


    calculateTextEditLayoutInfo();

    showTranslationPopup("", TextStyle::PlainText);

    show();
    raise();
    activateWindow();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
    qApp->removeEventFilter(this);

    delete ui;
}

void SimpleTranslatePopup::completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    ITranslateWidget::completeTransText(inTranslatedText, inTextStyle);
    _loadingBar->stop();
}

void SimpleTranslatePopup::applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    showTranslationPopup(inTranslatedText, inTextStyle);
}

QScrollBar* SimpleTranslatePopup::getVerticalScrollBar()
{
    return ui->resultText->verticalScrollBar();
}

QScrollBar* SimpleTranslatePopup::getHorizontalScrollBar()
{
    return ui->resultText->horizontalScrollBar();
}

QTextCursor SimpleTranslatePopup::getTextCursor()
{
    return ui->resultText->textCursor();
}

void SimpleTranslatePopup::setTextCursor(const QTextCursor& cursor)
{
    ui->resultText->setTextCursor(cursor);
}


void SimpleTranslatePopup::showTranslationPopup(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    if ((_prevSize.width() < _maxEditSize.width())
        || (_prevSize.height() < _maxEditSize.height()))
    {
        const QSize newSize = calculateTextEditSize(inTranslatedText);
        animateTextEditResize(newSize);
    }

    switch (inTextStyle)
    {
    case TextStyle::None:
        break;
    case TextStyle::PlainText:
        ui->resultText->setText(inTranslatedText);
        break;
    case TextStyle::Html:
        ui->resultText->setHtml(inTranslatedText);
        break;
    case TextStyle::MarkDown:
    {
        setMarkdown(inTranslatedText);
        break;
    }
    case TextStyle::Size:
        break;
    default: ;
    }

    // 문단간 간격 조정.
    const QFontMetricsF fntMetricsF(ui->resultText->font());
    const qreal lineHeight = fntMetricsF.lineSpacing();
    const qreal parSpacing = lineHeight * 0.6; // 줄간격의 1.6배

    const QTextDocument* doc = ui->resultText->document();
    QTextBlock block         = doc->firstBlock();
    
    while (block.isValid() && block.next().isValid())
    {
        QTextCursor blockCursor(block);
        QTextBlockFormat blockFormat = blockCursor.blockFormat();

        // 코드 블록은 간격 조정 안함.
        if (blockFormat.background().color() == codeBgColor
            && block.next().blockFormat().background().color() == codeBgColor)
        {
            break;
        }

        blockFormat.setBottomMargin(parSpacing);
        blockCursor.setBlockFormat(blockFormat);

        block = block.next();
    }
}

void SimpleTranslatePopup::setMarkdown(const QString& inMarkdownStr)
{
    QTextDocument* doc = ui->resultText->document();

    // 링크와 코드블록을 마크다운 스타일에서 html 스타일로 변경
    QString md = inMarkdownStr;

    // 이스케이프 되지 않은 <>가 태그로 인식되는 문제 해결
    static const QRegularExpression unescapedLT(R"((?<!\\)<)");
    md.replace(unescapedLT, R"(\<)");
    static const QRegularExpression unescapedGT(R"((?<!\\)>)");
    md.replace(unescapedGT, R"(\>)");

    QStringList monoFontList = doc->defaultFont().families();
    if (monoFontList.size() >= 2)
    {
        monoFontList.swapItemsAt(0, 1);
    }

    QString codeFontFamilies = " font-family: ";
    for (QString& font : monoFontList)
    {
        codeFontFamilies += "\'" + font + "\', ";
    }
    codeFontFamilies += ";";

    const QRegularExpression mdLinkPattern(R"(\[([^\]]+)\]\(([^)]+)\))");

    // 문단 코드
    const QRegularExpression codeQuotingPattern("```(.*?)```", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = codeQuotingPattern.globalMatch(md);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();

        QString original  = match.captured(0); // 백틱 포함 전체 패턴 일치
        QString codeBlock = match.captured(1); // 백틱 내부만

        // 코드 내부에 링크가 있다면, html 스타일 링크로 변경
        QString modified = codeBlock.toHtmlEscaped();
        modified.replace(mdLinkPattern, "<a href=\"\\2\"><code style= \"" + codeFontFamilies + " \"" " >\\1</code></a>");

        // 원래 코드 블록 전체를 수정된 내용으로 대체
        // 백틱을 html 스타일 코드 인용으로 변경
        md.replace(original, "\n<pre style=\"white-space: pre-wrap; background-color:" + codeBgColorStr + "; " + codeFontFamilies + " \">\n" + modified + "</pre>");
    }

    // 단어 코드 스니펫
    const QRegularExpression codePattern("`(.*?)`", QRegularExpression::DotMatchesEverythingOption);
    it = codePattern.globalMatch(inMarkdownStr);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();

        QString original  = match.captured(0); // 백틱 포함 전체 패턴 일치
        QString codeBlock = match.captured(1); // 백틱 내부만

        // 코드 내부에 링크가 있다면, html 스타일 링크로 변경
        QString modified = codeBlock.toHtmlEscaped();
        modified.replace(mdLinkPattern, "<a href=\"\\2\"><code style= \"" + codeFontFamilies + " \"" " >\\1</code></a>");

        // 원래 코드 블록 전체를 수정된 내용으로 대체
        // 백틱을 html 스타일 코드 인용으로 변경
        md.replace(original, "<code style= \"" + codeFontFamilies + "background-color:" + codeBgColorStr + "; \">" + modified + "</code>");
    }

    doc->setMarkdown(md);

}

void SimpleTranslatePopup::setTextEditSize(const QSize& inTextEditSize)
{
    // size
    const QSize bgFrameSize = inTextEditSize + _innerMarginSize;
    const QSize widgetSize  = bgFrameSize + _outerMarginSize;

    // text edit 폭 줄어드는 현상 방지.
    ui->resultText->setFixedSize(inTextEditSize);
    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    // 생성될 스크린 위치 추적
    QPointF screenTopLeft  = QPointF();
    QScreen* currentScreen = nullptr;

    const Fin::ScreenPopupPolicy screenPolicy = finConfig.getSimplePopupScreenPolicy();
    switch (screenPolicy)
    {
    case Fin::ScreenPopupPolicy::Default:
    case Fin::ScreenPopupPolicy::PrimaryScreen:
        currentScreen = qApp->primaryScreen();
        break;
    case Fin::ScreenPopupPolicy::FixedScreen:
        currentScreen = qApp->primaryScreen(); // 임시. 추후 저장된 스크린 위치 사용
        break;
    case Fin::ScreenPopupPolicy::CursorScreen:
        currentScreen = qApp->screenAt(QCursor::pos());
        break;
    case Fin::ScreenPopupPolicy::Size:
        break;
    }

    screenTopLeft = currentScreen ? currentScreen->geometry().topLeft() : QPointF();

    // position
    const QSizeF screenSize   = currentScreen ? currentScreen->size().toSizeF() : QSizeF(1920, 1080);
    const QPoint targetCenter = screenTopLeft.toPoint() + QPoint(screenSize.width() * _centerPosRatio.x(), screenSize.height() * _centerPosRatio.y());
    const QPoint recCenter    = rect().center();

    QPoint targetPos = targetCenter - recCenter;
    targetPos.rx() = qMin(targetPos.x(), static_cast<int>(screenSize.width() - widgetSize.width()));
    targetPos.ry() = qMax(targetPos.y(), static_cast<int>(screenSize.height()* _yPosMaxRatio));
    
    move(targetPos);

    update();
}

void SimpleTranslatePopup::manualSizeMode()
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

    const QSizeF screenSize = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);

    const QSize widgetMin = QSize(screenSize.width() * _minSizeRatio.width(), screenSize.height() * _minSizeRatio.height());
    const QSize widgetMax = QSize(screenSize.width() * _fullSizeRatio.width(), screenSize.height() * _fullSizeRatio.height());

    const QSize bgFrameMin = widgetMin - _outerMarginSize;
    const QSize bgFrameMax = widgetMax - _outerMarginSize;

    const QSize textMin = bgFrameMin - _innerMarginSize;
    const QSize textMax = bgFrameMax - _innerMarginSize;

    setMinimumSize(widgetMin);
    setMaximumSize(widgetMax);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    ui->bgFrame->setMinimumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->bgFrame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->resultText->setMinimumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->resultText->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void SimpleTranslatePopup::setupUI()
{
    ui->setupUi(this);

    ui->bgFrame->setLayout(ui->mainLayout);
    setLayout(ui->outerLayout);

    // ~===========
    // bgFrame shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
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
    _AlwaysOnButton = new QPushButton(this);
    _AlwaysOnButton->setCheckable(true);
    _AlwaysOnButton->setObjectName("alwaysOnButton");
    QIcon alwaysIcon;
    alwaysIcon.addFile(":/img/keep_pin_clock45d", QSize(), QIcon::Normal, QIcon::Off);
    alwaysIcon.addFile(":/img/keep_pin_fill_v", QSize(), QIcon::Normal, QIcon::On);
    _AlwaysOnButton->setIcon(alwaysIcon);
    _AlwaysOnButton->hide();

    setupTitleButton(_AlwaysOnButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_AlwaysOnButton, &QPushButton::toggled
          , this, &SimpleTranslatePopup::onAlwaysOnToggle);
    FinTooltipFilter::setCheckableButtonToolTip(_AlwaysOnButton, tr("항상 위 켜기"), tr("항상 위 끄기"));

    // ~===========
    // windowModeButton
    _windowModeButton = new QPushButton(this);
    _windowModeButton->setCheckable(true);
    _windowModeButton->setObjectName("windowModeButton");
    _windowModeButton->setIcon(QIcon(":/img/window_mode_img"));

    setupTitleButton(_windowModeButton, Qt::AlignTop | Qt::AlignLeft);

    connect(_windowModeButton, &QPushButton::toggled
          , this, &SimpleTranslatePopup::onWindowModeToggle);
    FinTooltipFilter::setCheckableButtonToolTip(_windowModeButton, tr("임시창을 일반창으로 승격"), tr("임시 창모드"));


    // 좌우 버튼 분리
    ui->titleLayout->setColumnStretch(getTitleLastColumn() - 1, 1);

    // ~===========
    // right side

    // ~===========
    // minimaize button    
    _minimizedButton = new QPushButton(this);
    _minimizedButton->setObjectName("minimizedButton");
    _minimizedButton->setIcon(QIcon(":/img/minimize_button_img"));

    setupTitleButton(_minimizedButton, Qt::AlignTop | Qt::AlignRight);
    FinTooltipFilter::setBubbleToolTip(_minimizedButton, tr("최소화"));
    connect(_minimizedButton, &QPushButton::clicked, this, &SimpleTranslatePopup::onMinimized);

    // ~===========
    // max button
    _maxRestoreButton = new QPushButton(this);
    _maxRestoreButton->setCheckable(true);
    _maxRestoreButton->setObjectName("maxRestoreButton");
    QIcon maxRestoreIcon;
    maxRestoreIcon.addFile(":/img/maximize_button_img", QSize(), QIcon::Normal, QIcon::Off);
    maxRestoreIcon.addFile(":/img/restore_button_img", QSize(), QIcon::Normal, QIcon::On);
    _maxRestoreButton->setIcon(maxRestoreIcon);

    setupTitleButton(_maxRestoreButton, Qt::AlignTop | Qt::AlignRight);
    connect(_maxRestoreButton, &QPushButton::toggled, this, &SimpleTranslatePopup::onMaxNormalToggle);
    FinTooltipFilter::setCheckableButtonToolTip(_maxRestoreButton, tr("최대화"), tr("이전 크기로 복원"));

    // ~===========
    // close button
    _closeButton = new QPushButton(this);
    _closeButton->setObjectName("closeButton");
    _closeButton->setIcon(QIcon(":/img/close_button_img"));
    _closeButton->setShortcut(Qt::Key_Escape);

    setupTitleButton(_closeButton, Qt::AlignTop | Qt::AlignRight);
    FinTooltipFilter::setBubbleToolTip(_closeButton, tr("닫기"));

    connect(_closeButton, &QPushButton::clicked, this, &QWidget::close);

    // top title layout end
    // ~===========

    // ~===========
    // bottom grip
    _sizeGrip = new QSizeGrip(this);
    ui->statusLayout->addWidget(_sizeGrip, 0, 1, Qt::AlignBottom | Qt::AlignRight);
    ui->statusLayout->setContentsMargins(0, 0, 4, 4);
    _sizeGrip->show();
    _sizeGrip->installEventFilter(this);


    _loadingBar = new LoadingBar(":/img/wait_anim_img", this);
    ui->loadingLayout->addWidget(_loadingBar, 0, 0);
    _loadingBar->run();

    // ~======================
    // resultText & scroll bar
    ui->textLayout->setContentsMargins(20, 0, 10, 20);

    QFont font = ui->resultText->font();
    font.setPointSizeF(_fontSize);
    ui->resultText->setFont(font);
    Qt::TextInteractionFlags interactionFlags = ui->resultText->textInteractionFlags();
    interactionFlags.setFlag(Qt::TextInteractionFlag::TextSelectableByMouse);
    interactionFlags.setFlag(Qt::TextInteractionFlag::TextSelectableByKeyboard);
    interactionFlags.setFlag(Qt::TextInteractionFlag::LinksAccessibleByMouse);
    interactionFlags.setFlag(Qt::TextInteractionFlag::LinksAccessibleByKeyboard);
    ui->resultText->setTextInteractionFlags(interactionFlags);
    ui->resultText->ensureCursorVisible();
    ui->resultText->setOpenExternalLinks(true);
    ui->resultText->setOpenLinks(true);
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

QSize SimpleTranslatePopup::calculateTextEditSize(const QString& inNewText) const
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

void SimpleTranslatePopup::animateTextEditResize(const QSize& inNewSize)
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

void SimpleTranslatePopup::adjustSizeAfterAnimationFinished()
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

void SimpleTranslatePopup::calculateTextEditLayoutInfo()
{
    // ~==============================
    // 단계별 마진 및 최소/최대 크기 계산
    if (screen() == nullptr)
    {
        qWarning() << "not detected screen";
    }
    const QSizeF screenSize  = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);
    const float minScreenLen = std::min(screenSize.width(), screenSize.height());

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
            + QMargins(0, 0, ui->outerVScrollBar->width(), 0);         // 우측 외부 스크롤바

    _outMargins = ui->outerLayout->contentsMargins();

    _innerMarginSize = QSize(inMargins.left() + inMargins.right() + ui->bgFrame->lineWidth()
                           , inMargins.top() + inMargins.bottom() + ui->bgFrame->lineWidth());
    _outerMarginSize = QSize(_outMargins.left() + _outMargins.right(), _outMargins.top() + _outMargins.bottom());


    const QSize totalMarginSize = _innerMarginSize + _outerMarginSize;

    _minEditSize = {minWidth - totalMarginSize.width(), minHeight - totalMarginSize.height()};
    _maxEditSize = {maxWidth - totalMarginSize.width(), maxHeight - totalMarginSize.height()};
}

void SimpleTranslatePopup::syncInOutScrollbar()
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

void SimpleTranslatePopup::onAlwaysOnToggle(bool checked)
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
        qDebug() << "AlwaysOn" << (checked ? "Top" : "NoTop") << "set failed";
    }
#else
    if (windowFlags().testFlag(Qt::WindowStaysOnTopHint) != checked)
    {
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        show();
    }
#endif
}

void SimpleTranslatePopup::onWindowModeToggle(bool checked)
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

void SimpleTranslatePopup::changeNormalWindowMode()
{
    // 자동닫기 해제
    qApp->removeEventFilter(this);

    if (_widgetModeFlags.testFlag(FinWidgetMode::PopupMode) == false)
    {
        return;
    }
    _widgetModeFlags.setFlag(FinWidgetMode::PopupMode, false);

    manualSizeMode();

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

void SimpleTranslatePopup::changePopupMode()
{
    // 팝업모드에서 자동 닫기 기능 등록
    qApp->installEventFilter(this);

    _widgetModeFlags.setFlag(FinWidgetMode::PopupMode);

    if (_AlwaysOnButton->isHidden() == false)
    {
        _AlwaysOnButton->hide();
    }
}

void SimpleTranslatePopup::setMaxNormal(const bool bMaximize)
{
    _maxRestoreButton->setChecked(bMaximize);
}

void SimpleTranslatePopup::onMaxNormalToggle(const bool bMaximize)
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

void SimpleTranslatePopup::onMinimized()
{
    manualSizeMode();
    changeNormalWindowMode();
    showMinimized();
}

void SimpleTranslatePopup::setShadowEffectEnabled(const bool bIsEnable)
{
    ui->bgFrame->graphicsEffect()->setEnabled(bIsEnable);
}

void SimpleTranslatePopup::detectFocusInOut(QWidget* old, QWidget* now)
{
    if (Fin::isThis(this, old))
    {
        if (Fin::isThis(this, now))
        {
            return;
        }
        else
        {
            setShadowEffectEnabled(false);
            return;
        }
    }
    if (Fin::isThis(this, now))
    {
        setShadowEffectEnabled(true);
    }
}

void SimpleTranslatePopup::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _dragPoint = event->globalPosition().toPoint() - frameGeometry().topLeft();
        _bIsDrag   = true;
        event->accept();
    }
}

void SimpleTranslatePopup::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        setMaxNormal(!_bMaximizedMode);

        event->accept();
    }
    
    QWidget::mouseDoubleClickEvent(event);  
}

void SimpleTranslatePopup::mouseMoveEvent(QMouseEvent* event)
{
    if (_bIsDrag && (event->button() | Qt::LeftButton))
    {
        manualSizeMode();

        const QPoint eventPoint = event->globalPosition().toPoint();
        if (_bMaximizedMode)
        {
            // 내부 QFrame의 절대 좌표와, QFrame 기준 상대 좌표 계산
            const QPoint outMarginTopLeft = QPoint(_outMargins.left(), _outMargins.top());
            const QPoint frameTopLeft     = frameGeometry().topLeft() + outMarginTopLeft;
            const QPointF mouseRelPointF  = (event->globalPosition().toPoint() - frameTopLeft).toPointF();

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


            const QPoint newNormalWindowPos = eventPoint - normalRelMousePoint - outMarginTopLeft;
            setMaxNormal(false);
            move(newNormalWindowPos);
            _dragPoint = eventPoint - newNormalWindowPos;
        }
        else
        {
            move(eventPoint - _dragPoint);
        }

        event->accept();
    }
}

void SimpleTranslatePopup::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _bIsDrag = false;
        event->accept();
    }
}

void SimpleTranslatePopup::enterEvent(QEnterEvent* event)
{
    setShadowEffectEnabled(true);

    QWidget::enterEvent(event);
}

void SimpleTranslatePopup::leaveEvent(QEvent* event)
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

    QWidget::leaveEvent(event);
}

bool SimpleTranslatePopup::eventFilter(QObject* obj, QEvent* event)
{
    // 팝업모드에서 자동 종료
    if (obj == qApp
        && _widgetModeFlags.testFlag(FinWidgetMode::PopupMode)
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
    }

    return QWidget::eventFilter(obj, event);
}
