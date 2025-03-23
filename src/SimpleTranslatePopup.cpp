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

#include "../ui/ui_SimpleTranslatePopup.h"


SimpleTranslatePopup::SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
    , finCore(inFinCore)
    , ui(new Ui::SimpleTranslatePopup)
{
    ui->setupUi(this);
    ui->bgFrame->setLayout(ui->textVLayout);
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

    calculateTextEditMax();

    // 애니메이션
    // 애니메이션 입력값으로 setTextEditSize 함수 호출 및 변경
    animation = new QPropertyAnimation(this, "textEditSize", this);
    animation->setDuration(250);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    showTranslationPopup(" ");

    show();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
    delete ui;
}


void SimpleTranslatePopup::showTranslationPopup(const QString& inTranslatedText)
{
    auto newSize = calculateTextEditSize(inTranslatedText);
    ui->resultText->setText(inTranslatedText);
    animateTextEditResize(newSize);
}

void SimpleTranslatePopup::setTextEditSize(const QSize& inTextEditSize)
{
    if (screen() == nullptr)
    {
        qWarning()<<"not detected screen";
    }

    const QSizeF screenSize = screen() ? screen()->size().toSizeF() : QSizeF(1920, 1080);

    ui->resultText->setFixedSize(inTextEditSize);

    const QSize bgFrameSize = inTextEditSize + innerMarginSize;
    const QSize widgetSize  = bgFrameSize + outerMarginSize;

    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    const QPoint targetCenter = QPoint(screenSize.width() * xPosRatio, screenSize.height() * yPosRatio);
    const QPoint recCenter    = rect().center();
    const QPoint targetPos    = targetCenter - recCenter;

    move(targetPos);
    ui->resultText->repaint();
}

void SimpleTranslatePopup::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPoint = event->globalPosition().toPoint() - frameGeometry().topLeft();
        bIsDrag   = true;
        event->accept();
    }
}

void SimpleTranslatePopup::mouseMoveEvent(QMouseEvent* event)
{
    if (bIsDrag && (event->button() | Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - dragPoint);
        event->accept();
    }
}

void SimpleTranslatePopup::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        bIsDrag = false;
        event->accept();
    }
}

void SimpleTranslatePopup::animateTextEditResize(const QSize& inNewSize)
{
    if (_prevSize == inNewSize)
    {
        return;
    }
    _prevSize = inNewSize;

    animation->setStartValue(ui->resultText->size());
    animation->setEndValue(inNewSize);
    animation->start();
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

    QMargins innerMargins = ui->textVLayout->contentsMargins();
    QMargins outerMargins = ui->outerVLayout->contentsMargins();

    innerMarginSize = QSize(innerMargins.left() + innerMargins.right(), innerMargins.top() + innerMargins.bottom());
    outerMarginSize = QSize(outerMargins.left() + outerMargins.right(), outerMargins.top() + outerMargins.bottom());

    const int frameLineWidth = ui->bgFrame->lineWidth() * 2;

    const QSize totalMarginSize = innerMarginSize + outerMarginSize;
    const int widthMargin       = totalMarginSize.width() + frameLineWidth;
    const int heightMargin      = totalMarginSize.height() + frameLineWidth;

    minEditSize = {minWidth - widthMargin, minHeight - heightMargin};
    maxEditSize = {maxWidth - widthMargin, maxHeight - heightMargin};


    // ui->resultText->setMinimumWidth(minEditSize.width());
    // ui->resultText->setMinimumHeight(minEditSize.height());
    // ui->resultText->setMaximumWidth(maxEditSize.width());
    // ui->resultText->setMaximumHeight(maxEditSize.height());
}

QSize SimpleTranslatePopup::calculateTextEditSize(const QString& inNewText)
{
    const QTextEdit* textEdit = ui->resultText;

    // counting added string's line break
    qsizetype idx = textEdit->toPlainText().length();
    while ((idx = inNewText.indexOf("\n\n", idx)) != -1)
    {
        ++idx;
        ++_lineBreakCount;
    }

    const QFontMetrics fntMetric = textEdit->fontMetrics();

    const int newStrWidth   = fntMetric.horizontalAdvance(inNewText);
    const int realLineCount = (newStrWidth + maxEditSize.width() - 1) / maxEditSize.width(); // (str너비 / 최대너비) 올림
    const int newLineCount  = realLineCount + _lineBreakCount + 1;
    const int lineHeight    = fntMetric.height();
    const int vMargin       = textEdit->contentsMargins().bottom() + textEdit->contentsMargins().top();
    const int newTextHeight = newLineCount * lineHeight + vMargin;

    const int newWidth  = qBound(minEditSize.width(), newStrWidth, maxEditSize.width());
    const int newHeight = qBound(minEditSize.height(), newTextHeight, maxEditSize.height());

    return QSize(newWidth, newHeight);
}
