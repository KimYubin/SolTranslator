#include "fintranslator.h"
#include "../ui_fintranslator.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


FinTranslator::FinTranslator(QWidget* parent) : QWidget(parent), ui(new Ui::FinTranslator)
{
    ui->setupUi(this);
    translateManager = std::make_unique<TranslateManager>(this);
    // loadTextFile();
}

FinTranslator::~FinTranslator()
{
    delete ui;
}

void FinTranslator::loadTextFile()
{
    QFile inputFile(":/save/textfinder.txt");
    inputFile.open(QIODevice::ReadOnly);

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui->textEditOrigin->setPlainText(line);
}

void FinTranslator::on_findButton_clicked()
{
    QString orignText = ui->textEditOrigin->toPlainText();
    translateManager->translateText(ui->textEditTranslate, orignText, "eng", "kor");
    
}
