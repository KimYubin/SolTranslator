//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslateWidget.h" resolved

#include "simpletranslatewidget.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qscreen.h>

#include "../ui_SimpleTranslateWidget.h"


SimpleTranslateWidget::SimpleTranslateWidget(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , ui(new Ui::SimpleTranslateWidget)
{
    ui->setupUi(this);

    // 구조 설정 및 위치 지정.
    boxLayout    = new QVBoxLayout(this);
    outTextLabel = new QLabel(this);
    outTextLabel->setStyleSheet("background-color: white; padding: 10px; border: 1px solid gray;");
    outTextLabel->setWordWrap(true);
    outTextLabel->setMaximumWidth(300);

    boxLayout->addWidget(outTextLabel);
    setLayout(boxLayout);

    move(QCursor::pos()); // 마우스 위치에 팝업 표시

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

    outTextLabel->setText(translatedText);
    if (QScreen* screen = QGuiApplication::primaryScreen())
    {
        int maxWidth = screen->size().width() / 5;
        outTextLabel->setMaximumWidth(maxWidth);
    }

    adjustSize(); // 내용에 맞게 창 크기 조절    
}
