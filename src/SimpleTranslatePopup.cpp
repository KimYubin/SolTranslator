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

#include "../ui/ui_SimpleTranslatePopup.h"


SimpleTranslatePopup::SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
    , _finCore(inFinCore)
    , ui(new Ui::SimpleTranslatePopup)
{
    ui->setupUi(this);

    ui->bgFrame->setLayout(ui->mainLayout);
    setLayout(ui->outerLayout);

    // bgFrame shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(14);
    shadow->setOffset(1, 1);
    shadow->setColor(QColor(0, 0, 0, 180));
    ui->bgFrame->setGraphicsEffect(shadow);

    // close button
    connect(ui->closeButton, &QPushButton::clicked, this, &QWidget::close);
    ui->closeButton->setFlat(true);

    // bottom grip
    _sizeGrip = new QSizeGrip(this);
    ui->statusLayout->addWidget(_sizeGrip, 0, 0, Qt::AlignBottom | Qt::AlignRight);
    ui->statusLayout->setContentsMargins(0, 0, 4, 4);
    

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
    ui->resultText->installEventFilter(this);

    // 애니메이션 setTextEditSize 함수 연결
    _animation = new QPropertyAnimation(this, "textEditSize", this);
    _animation->setDuration(250);
    _animation->setEasingCurve(QEasingCurve::OutQuad);

    calculateTextEditLayoutInfo();

    showTranslationPopup("");

    show();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
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
        qWarning()<<"not detected screen";
    }

    const QSizeF screenSize = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);

    const QSize bgFrameSize = inTextEditSize + _innerMarginSize;
    const QSize widgetSize  = bgFrameSize + _outerMarginSize;

    ui->resultText->setFixedSize(inTextEditSize);
    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    const QPoint targetCenter = QPoint(screenSize.width() * _centerPosRatio.x(), screenSize.height() * _centerPosRatio.y());
    const QPoint recCenter    = rect().center();

    QPoint targetPos    = targetCenter - recCenter;
    targetPos.rx() = qMin(targetPos.x(), static_cast<int>(screenSize.width() - widgetSize.width()));
    targetPos.ry() = qMax(targetPos.y(), static_cast<int>(screenSize.height()* _yPosMaxRatio));
    
    move(targetPos);
    ui->resultText->repaint();
}

void SimpleTranslatePopup::changeFixedMode()
{
    if (_bNonPopupMode)
    {
        return;
    }
    _bNonPopupMode = true;
    
    // ~==================
    // 창 종류 변경
    Qt::WindowFlags wflags = windowFlags();
    wflags &= ~Qt::Popup;
    setWindowFlags(wflags);
    show();

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

QSize SimpleTranslatePopup::calculateTextEditSize(const QString& inNewText)
{
    const QTextEdit* textEdit = ui->resultText;

    const QFontMetrics fntMetric = textEdit->fontMetrics();

    const int newStrWidth = fntMetric.horizontalAdvance(inNewText);

    const int docMarginTwice = static_cast<int>(textEdit->document()->documentMargin() * 2.0);
    const int viewportMargin = textEdit->size().width() - textEdit->viewport()->size().width();
    
    // pseudo Unicode line breaking
    const int maxWidth = _maxEditSize.width() - viewportMargin - docMarginTwice;
    if ((_lastLineLength + newStrWidth) >= maxWidth)
    {
        const int prevStrSize      = textEdit->toPlainText().length();
        const QString addedSubStr  = inNewText.sliced(prevStrSize);
        QTextBoundaryFinder textBF = {QTextBoundaryFinder::Word, addedSubStr};

        const int addSubStrSize = addedSubStr.length();
        qsizetype currentIdx    = textBF.position();
        while (currentIdx < addSubStrSize)
        {
            const qsizetype prevIdx = currentIdx;
            currentIdx = textBF.toNextBoundary();

            QString word = addedSubStr.sliced(prevIdx, currentIdx - prevIdx);
            if (word == "\n")
            {
                ++_lineCount;
                _lastLineLength = 0;
                continue;
            }

            const int wordWidth   = fntMetric.horizontalAdvance(word);
            const int lastWordLen = (_lastLineLength + wordWidth);
            if (lastWordLen >= maxWidth)
            {
                // line over
                ++_lineCount;

                // single word spans more than two lines
                auto [addLineCount, lastLineRem] = std::div(wordWidth, maxWidth);
                _lineCount += addLineCount;
                _lastLineLength = lastLineRem;
            }
            else
            {
                _lastLineLength += wordWidth;
            }
        }
    }

    const int lineHeight = fntMetric.height() * 1.1f;
    const int vMargin    = textEdit->contentsMargins().bottom() + textEdit->contentsMargins().top();

    const int newTextHeight = (_lineCount + (!!(_lastLineLength)) + 1) * lineHeight + vMargin;

    const int newWidth  = qBound(_minEditSize.width(), newStrWidth + docMarginTwice, _maxEditSize.width());
    const int newHeight = qBound(_minEditSize.height(), newTextHeight + docMarginTwice, _maxEditSize.height());

    return QSize(newWidth, newHeight);
}

void SimpleTranslatePopup::animateTextEditResize(const QSize& inNewSize)
{
    if (_prevSize == inNewSize)
    {
        return;
    }
    _prevSize = inNewSize;

    _animation->setStartValue(ui->resultText->size());
    _animation->setEndValue(inNewSize); // setTextEditSize
    _animation->start();
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
            changeFixedMode();
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
    if (obj == ui->resultText && event->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent(static_cast<QMouseEvent*>(event));
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
