//
// Created by YubinKim on 25/03/12 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SimpleTranslateWidget.h" resolved

#include "simpletranslatewidget.h"

#include <QLabel>
#include <qboxlayout.h>

#include "../ui_SimpleTranslateWidget.h"


SimpleTranslateWidget::SimpleTranslateWidget(const QString& translatedText, QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , ui(new Ui::SimpleTranslateWidget)
{
    ui->setupUi(this);
    showTranslationPopup(translatedText);
    show();
}

SimpleTranslateWidget::~SimpleTranslateWidget()
{
    delete ui;
}


void SimpleTranslateWidget::showTranslationPopup(const QString& translatedText)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label       = new QLabel(translatedText, this);
    label->setStyleSheet("background-color: white; padding: 10px; border: 1px solid gray;");

    layout->addWidget(label);
    setLayout(layout);

    adjustSize();         // 내용에 맞게 창 크기 조절
    move(QCursor::pos()); // 마우스 위치에 팝업 표시
    
}
