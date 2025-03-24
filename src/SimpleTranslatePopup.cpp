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
#include <qregularexpression.h>
#include <QTextBoundaryFinder>
#include <QScrollBar>

#include "../ui/ui_SimpleTranslatePopup.h"


SimpleTranslatePopup::SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
    , _finCore(inFinCore)
    , ui(new Ui::SimpleTranslatePopup)
{
    ui->setupUi(this);
    ui->bgFrame->setLayout(ui->textHLayout);
    setLayout(ui->outerVLayout);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(14);
    shadow->setOffset(1, 1);
    shadow->setColor(QColor(0, 0, 0, 180));

    ui->bgFrame->setGraphicsEffect(shadow);

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->resultText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // manual word wrap adjustsize 
    ui->resultText->setLineWrapMode(QTextEdit::LineWrapMode::FixedPixelWidth);

    calculateTextEditMax();

    // 애니메이션
    // 애니메이션 입력값으로 setTextEditSize 함수 호출 및 변경
    _animation = new QPropertyAnimation(this, "textEditSize", this);
    _animation->setDuration(250);
    _animation->setEasingCurve(QEasingCurve::OutCubic);

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

    ui->resultText->setFixedSize(inTextEditSize);

    // soft wrap width without scrollbar area
    ui->resultText->setLineWrapColumnOrWidth(ui->resultText->viewport()->size().width() - ui->resultText->verticalScrollBar()->width());

    const QSize bgFrameSize = inTextEditSize + _innerMarginSize;
    const QSize widgetSize  = bgFrameSize + _outerMarginSize;

    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    const QPoint targetCenter = QPoint(screenSize.width() * xPosRatio, screenSize.height() * yPosRatio);
    const QPoint recCenter    = rect().center();
    const QPoint targetPos    = targetCenter - recCenter;

    move(targetPos);
    ui->resultText->repaint();
}

QSize SimpleTranslatePopup::calculateTextEditSize(const QString& inNewText)
{
    const QTextEdit* textEdit = ui->resultText;

    const QFontMetrics fntMetric = textEdit->fontMetrics();

    const int newStrWidth = fntMetric.horizontalAdvance(inNewText);

    const int viewportMargin = ui->resultText->size().width() - ui->resultText->viewport()->size().width();

    // pseudo Unicode line breaking
    const int maxWidth = _maxEditSize.width() - textEdit->verticalScrollBar()->width() - viewportMargin;
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

    const int newWidth  = qBound(_minEditSize.width(), newStrWidth, _maxEditSize.width());
    const int newHeight = qBound(_minEditSize.height(), newTextHeight, _maxEditSize.height());

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

void SimpleTranslatePopup::calculateTextEditMax()
{
    if (screen() == nullptr)
    {
        qWarning() << "not detected screen";
    }
    const QSizeF screenSize  = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);
    const float minScreenLen = std::min(screenSize.width(), screenSize.height());

    const int maxWidth  = screenSize.width() * widthRatio;
    const int maxHeight = screenSize.height() * heightRatio;
    const int minWidth  = screenSize.width() * 0.15f;
    const int minHeight = screenSize.height() * 0.15f;

    QMargins innerMargins = ui->textHLayout->contentsMargins();
    QMargins outerMargins = ui->outerVLayout->contentsMargins();

    _innerMarginSize = QSize(innerMargins.left() + innerMargins.right(), innerMargins.top() + innerMargins.bottom());
    _outerMarginSize = QSize(outerMargins.left() + outerMargins.right(), outerMargins.top() + outerMargins.bottom());

    const int frameLineWidth = ui->bgFrame->lineWidth() * 2;

    const QSize totalMarginSize = _innerMarginSize + _outerMarginSize;
    const int widthMargin       = totalMarginSize.width() + frameLineWidth;
    const int heightMargin      = totalMarginSize.height() + frameLineWidth;

    _minEditSize = {minWidth - widthMargin, minHeight - heightMargin};
    _maxEditSize = {maxWidth - widthMargin, maxHeight - heightMargin};


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
