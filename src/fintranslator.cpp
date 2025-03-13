#include "fintranslator.h"
#include "../ui_fintranslator.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "ConfigManager.h"
#include "FinTypes.h"
#include "GlobalHotKeyManager.h"
#include "TranslateManager.h"


FinTranslator::FinTranslator(QWidget* parent) : QWidget(parent), ui(new Ui::FinTranslator)
{
    ui->setupUi(this);
    translateManager = new TranslateManager(this);
    globalHotKeyManager = new GlobalHotKeyManager(this);
    loadSettings();
}

FinTranslator::~FinTranslator()
{
    delete ui;
}

void FinTranslator::onSimpleTranslate(const QString& InOriginText)
{
    translateManager->translateSimple(InOriginText, Langs::ENGLISH.Name, Langs::KOREAN.Name);
}

void FinTranslator::loadSettings()
{
    loadAPI();
}

void FinTranslator::loadAPI()
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

void FinTranslator::on_findButton_clicked()
{
    loadAPI();

    QString orignText = ui->textEditOrigin->toPlainText();
    translateManager->translateText(ui->textEditTranslate, orignText, Langs::ENGLISH.Name, Langs::KOREAN.Name);
}
