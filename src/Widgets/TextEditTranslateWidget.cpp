//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TextEditTranslateWidget.h" resolved

#include "TextEditTranslateWidget.h"

#include <QPointer>

#include "../FinTypes.h"
#include "../ConfigManager.h"
#include "../FinTranslatorCore.h"
#include "../TranslateManager.h"

#include "Widgets/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);
    
    setLayout(ui->mainLayout);

    ui->textOrigin->setTabChangesFocus(true);
    ui->textTranslate->setTabChangesFocus(true);
    ui->textTranslate->setReadOnly(true);
    ui->textTranslate->setMouseTracking(false);
    QFont textFont = ui->textTranslate->font();
    textFont.setHintingPreference(QFont::PreferDefaultHinting);
    ui->textTranslate->setFont(textFont);
    ui->textTranslate->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard |
        Qt::TextEditable
    );

}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::focusTextOrigin()
{
    ui->textOrigin->setFocus();
}

void TextEditTranslateWidget::on_findButton_clicked()
{
    const QString orignText = ui->textOrigin->toPlainText();

    finCore->getTranslateManager()->translateText(TranslateRequestInfo{
        orignText
      , TextStyle::PlainText
      , LangType::en
      , LangType::ko
      , ui->textTranslate
      , [=](const QString& inStr) { ui->textTranslate->setPlainText(inStr); }
      , ui->textTranslate
      , [=](const QString& inStr) { ui->textTranslate->setPlainText(inStr); }
    });
}

