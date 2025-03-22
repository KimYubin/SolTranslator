//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslatePopup.h" resolved

#include "SimpleTranslatePopup.h"

#include <qboxlayout.h>
#include <qevent.h>
#include <qscreen.h>
#include <QGraphicsDropShadowEffect>


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

    showTranslationPopup(" ");

    show();
}

SimpleTranslatePopup::~SimpleTranslatePopup()
{
    delete ui;
}


void SimpleTranslatePopup::showTranslationPopup(const QString& inTranslatedText)
{
    const int prevTextWidth = ui->resultText->document()->size().width();
    const int prevTextHeight = ui->resultText->document()->size().height();
    
    ui->resultText->setText(inTranslatedText);

    if (QScreen* screen = QGuiApplication::primaryScreen())
    {
        const float screenWidthf  = static_cast<float>(screen->size().width());
        const float screenHeightf = static_cast<float>(screen->size().height());
        const float minScreenLength = std::min(screenWidthf, screenHeightf);

        calculateTextEditSize();

        if (prevTextWidth < maxTextEditSize.width())
        {
            ui->resultText->document()->adjustSize();

            const int docWidth = ui->resultText->document()->size().width();
            int desiredWidth   = qMax(prevTextWidth, qBound(minTextEditSize.width(), docWidth, maxTextEditSize.width()));
            ui->resultText->setFixedWidth(desiredWidth);

            ui->resultText->repaint();
        }

        if (prevTextHeight < maxTextEditSize.height())
        {
            // 너비 조정 후 높이 조정
            const int docHeight     = ui->resultText->document()->size().height();
            const int desiredHeight = qMax(prevTextHeight, qBound(minTextEditSize.height(), docHeight, maxTextEditSize.height()));
            ui->resultText->setFixedHeight(desiredHeight);
        }
        QSize bgFrameSize = ui->resultText->size() + QSize{innerMargin.left() + innerMargin.right(), innerMargin.top() + innerMargin.bottom()};
        QSize widgetSize  = bgFrameSize + QSize{outerMargin.left() + outerMargin.right(), outerMargin.top() + outerMargin.bottom()};

        ui->bgFrame->setFixedSize(bgFrameSize);
        setFixedSize(widgetSize);

        QPoint targetCenter = QPoint(screenWidthf * xPosRatio, screenHeightf * yPosRatio);
        QPoint recCenter    = rect().center();
        QPoint targetPos    = targetCenter - recCenter;

        move(targetPos);
    }
    else
    {
        move(QCursor::pos());
    }

    // QApplication::processEvents();
    // update();
    // 네트워크 대기로 인한 지연된 업데이트 탈출
    repaint();
}

void SimpleTranslatePopup::addTranslationText(const QString& inTranslatedText)
{
    
}

void SimpleTranslatePopup::completeText(const QString& inTranslatedText) {
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

void SimpleTranslatePopup::calculateTextEditSize()
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
