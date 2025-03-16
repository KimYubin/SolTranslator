//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslateWidget.h" resolved

#include "simpletranslatewidget.h"

#include <qboxlayout.h>
#include <qevent.h>
#include <qscreen.h>

#include "../ui/ui_SimpleTranslateWidget.h"


SimpleTranslateWidget::SimpleTranslateWidget(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , ui(new Ui::SimpleTranslateWidget)
{
    ui->setupUi(this);
    
    setLayout(ui->boxLayout);

    // 구조 설정 및 위치 지정.
    // ui->outTextLabel->setStyleSheet("background-color: white; padding: 0px; border: 0px solid gray;");
    showTranslationPopup(" ");

    show();
}

SimpleTranslateWidget::~SimpleTranslateWidget()
{
    delete ui;
}


void SimpleTranslateWidget::showTranslationPopup(const QString& translatedText)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->outTextLabel->setText(translatedText);
    adjustSize();

    constexpr float widthRatio  = 0.20f;
    constexpr float heightRatio = 0.6f;
    constexpr float xPosRatio   = 0.85f;
    constexpr float yPosRatio   = 0.35f;
    
    if (QScreen* screen = QGuiApplication::primaryScreen())
    {
        const float screenWidthf  = static_cast<float>(screen->size().width());
        const float screenHeightf = static_cast<float>(screen->size().height());

        const int maxWidth  = screenWidthf * widthRatio;
        const int maxHeight = screenHeightf * heightRatio;
        setMaximumWidth(maxWidth);
        setMaximumHeight(maxHeight);

        ui->outTextLabel->setWordWrap(true);

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        QPoint targetCenterPos    = QPoint(screenWidthf * xPosRatio, screenHeightf * yPosRatio);
        QPoint widgetRelCenterPos = rect().center();
        QPoint targetPos          = targetCenterPos - widgetRelCenterPos;

        move(targetPos);
    }
    else
    {
        move(QCursor::pos());
    }
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
    if (bIsDrag && (event->button() == Qt::LeftButton))
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
