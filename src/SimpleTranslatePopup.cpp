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

    // 애니메이션 설정
    animation = new QPropertyAnimation(this, "textEditSize", this);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(ui->resultText, &QTextEdit::textChanged, this, &SimpleTranslatePopup::animateResize);
    
    showTranslationPopup(" ");

    show();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
    delete ui;
}


void SimpleTranslatePopup::showTranslationPopup(const QString& inTranslatedText)
{
    ui->resultText->setText(inTranslatedText);

    if (QScreen* screen = QGuiApplication::primaryScreen())
    {

    }
    else
    {
        move(QCursor::pos());
    }

    // QApplication::processEvents();
    // update();
    // 네트워크 대기로 인한 지연된 업데이트 탈출
    // repaint();
}

void SimpleTranslatePopup::addTranslationText(const QString& inTranslatedText)
{
    
}

void SimpleTranslatePopup::completeText(const QString& inTranslatedText) {
}

void SimpleTranslatePopup::setTextEditSize(const QSize& inTextEditSize)
{
    QScreen* screen           = QGuiApplication::primaryScreen();
    const float screenWidthf  = static_cast<float>(screen->size().width());
    const float screenHeightf = static_cast<float>(screen->size().height());

    ui->resultText->setFixedSize(inTextEditSize);
    
    QSize bgFrameSize = inTextEditSize + QSize{innerMargin.left() + innerMargin.right(), innerMargin.top() + innerMargin.bottom()};
    QSize widgetSize  = bgFrameSize + QSize{outerMargin.left() + outerMargin.right(), outerMargin.top() + outerMargin.bottom()};

    ui->bgFrame->setFixedSize(bgFrameSize);
    setFixedSize(widgetSize);

    QPoint targetCenter = QPoint(screenWidthf * xPosRatio, screenHeightf * yPosRatio);
    QPoint recCenter    = rect().center();
    QPoint targetPos    = targetCenter - recCenter;

    move(targetPos);
    ui->resultText->repaint();
}

void SimpleTranslatePopup::setTextEditPos(const QPoint& inTextEditPos)
{
    
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

void SimpleTranslatePopup::animateResize()
{
    QTextDocument* doc = ui->resultText->document();
    
    QSizeF docSize = ui->resultText->document()->size();
    docSize.setWidth(qMax(docSize.width(), static_cast<qreal>(_prevSize.width())));
    int targetWidth   = qBound(qMax(minTextEditSize.width(), _prevSize.width()), static_cast<int>(docSize.width()) + 10, maxTextEditSize.width());
    // 2. 문서 레이아웃 강제 적용
    doc->setTextWidth(targetWidth);  // 가로 기준으로 레이아웃 계산

    // 3. 갱신된 세로 크기 계산
     docSize = doc->size();
    int targetHeight = qBound(minTextEditSize.height(),
                              static_cast<int>(docSize.height()) + 10,
                              maxTextEditSize.height());

    // 4. 목표 크기 설정
    QSize newSize(targetWidth, targetHeight);

    newSize = calculateTextEditSize();

    if (_prevSize == newSize)
        return;
    
    _prevSize = newSize;
    
    // 5. 애니메이션 실행 (중복 방지 포함)
    if (animation->state() == QAbstractAnimation::Running) {
        // animation->stop();
    }

    animation->setStartValue(ui->resultText->size());
    animation->setEndValue(newSize);
    animation->start();
}

void SimpleTranslatePopup::calculateTextEditMax()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen == nullptr)
        return;

    const float screenWidthf    = static_cast<float>(screen->size().width());
    const float screenHeightf   = static_cast<float>(screen->size().height());
    const float minScreenLength = std::min(screenWidthf, screenHeightf);

    const int maxWidth  = screenWidthf * widthRatio;
    const int maxHeight = screenHeightf * heightRatio;
    const int minWidth  = screenWidthf * 0.15f;
    const int minHeight = screenHeightf * 0.15f;
    setMaximumWidth(maxWidth);
    setMaximumHeight(maxHeight);
    setMinimumWidth(minWidth);
    setMinimumHeight(minHeight);

    // ~==========
    // text edit size
    outerMargin = ui->outerVLayout->contentsMargins();
    innerMargin = ui->textVLayout->contentsMargins();
    frameLineWidth = ui->bgFrame->lineWidth();

    const QMargins totalMargin = outerMargin + innerMargin + (frameLineWidth * 2);
    const int widthMargin      = (totalMargin.left() + totalMargin.right());
    const int heightMargin     = (totalMargin.top() + totalMargin.bottom());

    minTextEditSize = {minWidth - widthMargin, minHeight - heightMargin};
    maxTextEditSize = {maxWidth - widthMargin, maxHeight - heightMargin};


    ui->resultText->setMinimumWidth(minTextEditSize.width());
    ui->resultText->setMinimumHeight(minTextEditSize.height());
    ui->resultText->setMaximumWidth(maxTextEditSize.width());
    ui->resultText->setMaximumHeight(maxTextEditSize.height());

}

QSize SimpleTranslatePopup::calculateTextEditSize(int margin)
{
    QTextEdit* textEdit = ui->resultText;

    const QString text = textEdit->toPlainText();
    const QFont font   = textEdit->font();
    const QFontMetrics fontMetrics(font);

    const int strWidth    = fontMetrics.horizontalAdvance(text); // 전체 너비
    const div_t lineCount = std::div(strWidth, minTextEditSize.width());

    const int totalLines = lineCount.quot + (lineCount.rem == 0 ? 0 : 1);

    const int lineHeight = fontMetrics.height();

    // 6. 텍스트가 차지하는 세로 크기 계산
    const int vMargin          = textEdit->contentsMargins().bottom() + textEdit->contentsMargins().top();
    const int calculatedHeight = totalLines * lineHeight + vMargin;

    // 7. 최종 크기 계산 (가로는 고정, 세로는 계산된 값)
    const int finalWidth  = qBound(minTextEditSize.width(), maxTextEditSize.width(), maxTextEditSize.width()); // 가로는 maxWidth로 고정
    const int finalHeight = qBound(minTextEditSize.height(), calculatedHeight, maxTextEditSize.height());      // 세로 크기 계산 후 최소, 최대 범위 적용

    return QSize(finalWidth, finalHeight); // 최종 계산된 크기 반환
}
