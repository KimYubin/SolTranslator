//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TextEditTranslateWidget.h" resolved

#include "TextEditTranslateWidget.h"

#include <QLineEdit>
#include <QPointer>
#include <QScrollBar>
#include <QTimer>

#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>

#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/SearchDropdown.h"

#include "Widgets/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    ui->hLayout_2_TextInputs->setSpacing(8);

    ui->originTextEdit->setTabChangesFocus(true);
    ui->trTextEdit->setTabChangesFocus(true);
    ui->trTextEdit->setReadOnly(true);
    ui->trTextEdit->setMouseTracking(false);
    ui->trTextEdit->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard |
        Qt::TextEditable
    );

    SearchDropdown* sourceLang = new SearchDropdown(this, ui->originTextEdit, finConfig.getTextSrcLang());
    connect(sourceLang, &SearchDropdown::languageSelected, this, [](const LangType inlangType)
    {
        finConfig.setTextSrcLang(inlangType);
    });
    ui->hLayout_1_LangSelect->insertWidget(0, sourceLang, 1);

    SearchDropdown* targetLang = new SearchDropdown(this, ui->trTextEdit, finConfig.getTextTargetLang());
    connect(targetLang, &SearchDropdown::languageSelected, this, [](const LangType inlangType)
    {
        finConfig.setTextTargetLang(inlangType);
    });
    ui->hLayout_1_LangSelect->insertWidget(2, targetLang, 1);

    connect(ui->translateButton, &QPushButton::clicked, this, &TextEditTranslateWidget::onTranslateClicked);


}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    ui->trTextEdit->setPlainText(inTranslatedText);
}

QScrollBar* TextEditTranslateWidget::getVerticalScrollBar()
{
    return ui->trTextEdit->verticalScrollBar();
}

QScrollBar* TextEditTranslateWidget::getHorizontalScrollBar()
{
    return ui->trTextEdit->horizontalScrollBar();
}

void TextEditTranslateWidget::focusTextOrigin()
{
    ui->originTextEdit->setFocus();
}

void TextEditTranslateWidget::onTranslateClicked()
{
    const QString orignText = ui->originTextEdit->toPlainText();

    finCore->getTranslateManager()->translateText(TranslateRequestInfo{
        orignText
      , TextStyle::PlainText
      , finConfig.getTextSrcLang()
      , finConfig.getTextTargetLang()
      , ui->trTextEdit
      , [this](const QString& inStr) { completeTransText(inStr, TextStyle::PlainText); }
      , ui->trTextEdit
      , [this](const QString& inStr) { streamTransText(inStr, TextStyle::PlainText); }
    });
}

