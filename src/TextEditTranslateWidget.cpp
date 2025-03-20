//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TextEditTranslateWidget.h" resolved

#include "TextEditTranslateWidget.h"

#include "ConfigManager.h"
#include "FinTranslatorCore.h"
#include "TranslateManager.h"
#include "FinTypes.h"

#include "../ui/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(FinTranslatorCore* inFinCore, QWidget* parent)
    : QWidget(parent)
    , finCore(inFinCore)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);
    
    setLayout(ui->mainLayout);

    loadSettings();
}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::on_findButton_clicked()
{
    loadAPI();

    const QString orignText = ui->plainTextEditOrigin->toPlainText();

    finCore->getTranslateManager()->translateText(ui->plainTextEditTranslate, &QPlainTextEdit::setPlainText, orignText, LangType::en, LangType::ko);
}

void TextEditTranslateWidget::loadSettings()
{
    loadAPI();
}

void TextEditTranslateWidget::loadAPI()
{
    QString newAPI = ui->lineEdit_api->text();
    if (newAPI.isEmpty())
    {
        QString oldAPI = ConfigManager::get().getAPI();
        if (oldAPI.isEmpty() == false)
        {
            QString asteriskAPI = oldAPI.first(3) + "***...";
            ui->lineEdit_api->setText(asteriskAPI);
        }
    }
    else
    {
        if (newAPI.last(6) != "***...")
        {
            ConfigManager::get().setAPI(newAPI);
        }
    }
}
