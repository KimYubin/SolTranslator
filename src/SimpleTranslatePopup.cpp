//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslatePopup.h" resolved

#include "SimpleTranslatePopup.h"

#include <iostream>
#include <QAbstractTextDocumentLayout>
#include <qboxlayout.h>
#include <qevent.h>
#include <qscreen.h>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <qregularexpression.h>
#include <QTextBoundaryFinder>
#include <QScrollBar>
#include <QSizeGrip>
#include <QCheckBox>

#include "../ui/ui_SimpleTranslatePopup.h"


SimpleTranslatePopup::SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , _finCore(inFinCore)
    , ui(new Ui::SimpleTranslatePopup)
{
    QIcon icon = QIcon(":/img/icon_img.png");
    setWindowIcon(icon);
    setWindowTitle(tr("fin"));

    setupUI();

    // ~===========
    // config
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->resultText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // ~======================
    // resultText & scroll bar
    // 기본 스크롤바 숨김
    ui->resultText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 외부 스크롤바 -> 내부 스크롤바 제어
    connect(ui->outerVScrollBar, &QScrollBar::valueChanged, this, [=](const int value)
    {
        ui->resultText->verticalScrollBar()->setValue(value);
    });

    // 내부 스크롤바 값 -> 외부 스크롤바에 반영
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::rangeChanged, this, [=](int, int)
    {
        syncInOutScrollbar();
    });
    connect(ui->resultText->verticalScrollBar(), &QScrollBar::valueChanged, this, [=](int)
    {
        syncInOutScrollbar();
    });
    // 문서 정보 반영
    connect(ui->resultText->document(), &QTextDocument::contentsChanged, this, [=]()
    {
        syncInOutScrollbar();
    });


    // ~======================
    // 애니메이션
    _animation = new QPropertyAnimation(this, "textEditSize", this); // setTextEditSize 함수 연결
    _animation->setDuration(250);
    _animation->setEasingCurve(QEasingCurve::OutQuad);
    connect(_animation, &QAbstractAnimation::finished, this, &SimpleTranslatePopup::adjustSizeAfterAnimationFinished);


    calculateTextEditLayoutInfo();

    showTranslationPopup("");

    show();
    raise();
    activateWindow();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
    qApp->removeEventFilter(this);
    delete ui;
}


void SimpleTranslatePopup::showTranslationPopup(const QString& inTranslatedText)
{
    if ((_prevSize.width() < _maxEditSize.width())
        || (_prevSize.height() < _maxEditSize.height()))
    {
        const QSize newSize = calculateTextEditSize(inTranslatedText);
        animateTextEditResize(newSize);
    }

    ui->resultText->setText(inTranslatedText);
}

void SimpleTranslatePopup::setTextEditSize(const QSize& inTextEditSize)
{
    if (screen() == nullptr)
    {
        qWarning() << "not detected screen";
    }
    // size
    const QSize bgFrameSize = inTextEditSize + _innerMarginSize;
    const QSize widgetSize  = bgFrameSize + _outerMarginSize;

    ui->resultText->setFixedSize(inTextEditSize);
    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    // position
    const QSizeF screenSize   = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);
    const QPoint targetCenter = QPoint(screenSize.width() * _centerPosRatio.x(), screenSize.height() * _centerPosRatio.y());
    const QPoint recCenter    = rect().center();

    QPoint targetPos = targetCenter - recCenter;
    targetPos.rx() = qMin(targetPos.x(), static_cast<int>(screenSize.width() - widgetSize.width()));
    targetPos.ry() = qMax(targetPos.y(), static_cast<int>(screenSize.height()* _yPosMaxRatio));
    
    move(targetPos);
    ui->resultText->repaint();
}

void SimpleTranslatePopup::changeNonPopupMode()
{
    // ~==================
    // 팝업 모드 해제
    if (_bPopupMode == false)
    {
        return;
    }
    _bPopupMode = false;

    // ~==================
    // 위젯 사이즈 변경 애니메이션 정지 및 해제
    _animation->stop();
    _animation->setPropertyName("");

    // ~==================
    // 위젯 사이즈 정책 변경
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    ui->bgFrame->setMinimumSize(bgFrameMin);
    ui->bgFrame->setMaximumSize(bgFrameMax);
    ui->resultText->setMinimumSize(textMin);
    ui->resultText->setMaximumSize(textMax);
}


void SimpleTranslatePopup::setupUI()
{
    ui->setupUi(this);

    ui->bgFrame->setLayout(ui->mainLayout);
    setLayout(ui->outerLayout);

    // bgFrame shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setOffset(0.5);
    shadow->setColor(QColor(0, 0, 0, 250));
    ui->bgFrame->setGraphicsEffect(shadow);

    // ~===========
    // close button
    ui->closeButton->setFlat(true);
    connect(ui->closeButton, &QPushButton::clicked, this, &SimpleTranslatePopup::onCloseWithManual);
    qApp->installEventFilter(this); // 팝업모드에서 자동 닫기 기능 등록

    // ~===========
    // keepPinButton
    _keepPinButton = new QPushButton(this);
    _keepPinButton->setCheckable(true);
    ui->titleLayout->addWidget(_keepPinButton, 0, 0, Qt::AlignTop | Qt::AlignLeft);
    _keepPinButton->setObjectName("keepPinButton");
    QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(_keepPinButton->sizePolicy().hasHeightForWidth());
    _keepPinButton->setSizePolicy(sizePolicy);
    _keepPinButton->setMinimumSize(QSize(24, 24));
    _keepPinButton->setMaximumSize(QSize(24, 24));
    _keepPinButton->setIcon(QIcon(":/img/keep_pin_clock45d"));
    _keepPinButton->setFlat(true);
    connect(_keepPinButton, &QPushButton::toggled, this, &SimpleTranslatePopup::onKeepPinButtonToggle);

    // ~===========
    // bottom grip
    _sizeGrip = new QSizeGrip(this);
    ui->statusLayout->addWidget(_sizeGrip, 0, 0, Qt::AlignBottom | Qt::AlignRight);
    ui->statusLayout->setContentsMargins(0, 0, 4, 4);
    _sizeGrip->hide();

}

QSize SimpleTranslatePopup::calculateTextEditSize(const QString& inNewText) const
{
    const QTextEdit* textEdit = ui->resultText;

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
        return;
    }
    _prevSize = inNewSize;

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
    // 외부 스크롤바 마진 적용.
    // 텍스트와 스크롤바가 겹치지 않게 합니다.
    ui->textLayout->activate();
    const qreal docMargin    = ui->resultText->document()->documentMargin();
    const int vScrollWidth   = ui->outerVScrollBar->width();
    const qreal newDocMargin = vScrollWidth > docMargin ? vScrollWidth : docMargin;
    ui->resultText->document()->setDocumentMargin(newDocMargin + 1);


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


    const QMargins inMargins = ui->textLayout->contentsMargins()
            + ui->mainLayout->contentsMargins()                         // 메인 컨텐츠 레이아웃 마진
            + QMargins(0, ui->titleLayout->sizeHint().height(), 0, 0)   // 상단 타이틀바 레이아웃 높이
            + QMargins(0, 0, 0, ui->statusLayout->sizeHint().height()); // 하단 상태표시 레이아웃 높이

    const QMargins outMargins = ui->outerLayout->contentsMargins();

    _innerMarginSize = QSize(inMargins.left() + inMargins.right(), inMargins.top() + inMargins.bottom());
    _outerMarginSize = QSize(outMargins.left() + outMargins.right(), outMargins.top() + outMargins.bottom());

    const int frameLineWidth = ui->bgFrame->lineWidth() * 2;

    const QSize totalMarginSize = _innerMarginSize + _outerMarginSize;
    const int widthMargin       = totalMarginSize.width() + frameLineWidth;
    const int heightMargin      = totalMarginSize.height() + frameLineWidth;

    _minEditSize = {minWidth - widthMargin, minHeight - heightMargin};
    _maxEditSize = {maxWidth - widthMargin, maxHeight - heightMargin};
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

void SimpleTranslatePopup::onKeepPinButtonToggle(bool checked)
{
    QString iconURL;
    if (checked)
    {
        changeNonPopupMode();
        _sizeGrip->show();

        iconURL = ":/img/keep_pin_v";
    }
    else
    {
        _bPopupMode = true;
        iconURL = ":/img/keep_pin_clock45d";
    }
    _keepPinButton->setIcon(QIcon(iconURL));
}

void SimpleTranslatePopup::onCloseWithManual()
{
    _bManualClose = true;
    close();
}

void SimpleTranslatePopup::closeEvent(QCloseEvent* event)
{
    if (_bPopupMode || _bManualClose)
    {
        QWidget::closeEvent(event);
    }
    else
    {
        event->ignore();
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

void SimpleTranslatePopup::mouseMoveEvent(QMouseEvent* event)
{
    if (_bIsDrag && (event->button() | Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - _dragPoint);
        event->accept();
        if (underMouse())
        {
            changeNonPopupMode();
        }
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

bool SimpleTranslatePopup::eventFilter(QObject* obj, QEvent* event)
{
    if (_bPopupMode && event->type() == QEvent::ApplicationStateChange)
    {
        Qt::ApplicationState changeState = static_cast<QApplicationStateChangeEvent*>(event)->applicationState();
        if (changeState != Qt::ApplicationActive)
        {
            close();
        }
        return true;
    }

    return QWidget::eventFilter(obj, event);
}
