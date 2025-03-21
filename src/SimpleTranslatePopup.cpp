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
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    ui->resultText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

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

    constexpr float widthRatio  = 0.20f;
    constexpr float heightRatio = 0.6f;
    constexpr float xPosRatio   = 0.85f;
    constexpr float yPosRatio   = 0.35f;

    if (QScreen* screen = QGuiApplication::primaryScreen())
    {
        const float screenWidthf  = static_cast<float>(screen->size().width());
        const float screenHeightf = static_cast<float>(screen->size().height());
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
        const QMargins outerMargin = ui->outerVLayout->contentsMargins();
        const QMargins innerMargin = ui->textVLayout->contentsMargins();
        const int frameMargin      = ui->bgFrame->lineWidth() * 2;

        const QMargins totalMargin = outerMargin + innerMargin + frameMargin;
        const int widthMargin      = (totalMargin.left() + totalMargin.right());
        const int heightMargin     = (totalMargin.top() + totalMargin.bottom());

        const QSize minTextEditSize = {minWidth - widthMargin, minHeight - heightMargin};
        const QSize maxTextEditSize = {maxWidth - widthMargin, maxHeight - heightMargin};

        ui->resultText->setMinimumWidth(minTextEditSize.width());
        ui->resultText->setMinimumHeight(minTextEditSize.height());
        ui->resultText->setMaximumWidth(maxTextEditSize.width());
        ui->resultText->setMaximumHeight(maxTextEditSize.height());

        ui->resultText->document()->adjustSize();

        const int docWidth     = ui->resultText->document()->size().width();
        const int desiredWidth = qBound(minTextEditSize.width(), docWidth, maxTextEditSize.width());
        ui->resultText->setFixedWidth(desiredWidth);

        ui->resultText->repaint();

        // 너비 조정 후 높이 조정
        const int docHeight     = ui->resultText->document()->size().height();
        const int desiredHeight = qBound(minTextEditSize.height(), docHeight, maxTextEditSize.height());
        ui->resultText->setFixedHeight(desiredHeight);


        ui->bgFrame->adjustSize();
        adjustSize();

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
