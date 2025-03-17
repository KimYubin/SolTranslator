//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslateWidget.h" resolved

#include "simpletranslatewidget.h"

#include <qboxlayout.h>
#include <qevent.h>
#include <qscreen.h>
#include <QGraphicsDropShadowEffect>

#include "../ui/ui_SimpleTranslateWidget.h"


SimpleTranslateWidget::SimpleTranslateWidget(QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint)
    , ui(new Ui::SimpleTranslateWidget)
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

    showTranslationPopup(" ");

    show();
}

SimpleTranslateWidget::~SimpleTranslateWidget()
{
    delete ui;
}


void SimpleTranslateWidget::showTranslationPopup(const QString& translatedText)
{
    ui->resultTextLabel->setWordWrap(false);
    ui->resultTextLabel->setText(translatedText);

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

        ui->resultTextLabel->setWordWrap(true);
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

void SimpleTranslateWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPoint = event->globalPosition().toPoint() - frameGeometry().topLeft();
        bIsDrag   = true;
        event->accept();
    }
}

void SimpleTranslateWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (bIsDrag && (event->button() | Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - dragPoint);
        event->accept();
    }
}

void SimpleTranslateWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        bIsDrag = false;
        event->accept();
    }
}
