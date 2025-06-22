//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TextEditTranslateWidget.h" resolved

#include "TextEditTranslateWidget.h"

#include <QPointer>
#include <QTimer>

#include "../FinTypes.h"
#include "../Managers/ConfigManager.h"
#include "../FinTranslatorCore.h"
#include "../Managers/TranslateManager.h"

#include "Widgets/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    ui->textOrigin->setTabChangesFocus(true);
    ui->textTranslate->setTabChangesFocus(true);
    ui->textTranslate->setReadOnly(true);
    ui->textTranslate->setMouseTracking(false);
    ui->textTranslate->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard |
        Qt::TextEditable
    );


    connect(ui->translateButton, &QPushButton::clicked, this, &TextEditTranslateWidget::onTranslateClicked);
}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    ui->textTranslate->setPlainText(inTranslatedText);
}

void TextEditTranslateWidget::focusTextOrigin()
{
    ui->textOrigin->setFocus();
}

void TextEditTranslateWidget::onTranslateClicked()
{
    const QString orignText = ui->textOrigin->toPlainText();

    finCore->getTranslateManager()->translateText(TranslateRequestInfo{
        orignText
      , TextStyle::PlainText
      , LangType::en
      , LangType::ko
      , ui->textTranslate
      , [this](const QString& inStr) { completeTransText(inStr, TextStyle::PlainText); }
      , ui->textTranslate
      , [this](const QString& inStr) { streamTransText(inStr, TextStyle::PlainText); }
    });
}

