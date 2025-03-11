//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H

#include <QCoreApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <unordered_set>

class QTextEdit;
class TranslateUnit;

class TranslateManager : public QObject
{
    Q_OBJECT

public:
    TranslateManager(QObject* parent = nullptr);

    void translateText(QTextEdit* inTextEditableObj
                       , const QString& text
                       , const QString& sourceLang
                       , const QString& targetLang);

    void removeUnit(const std::shared_ptr<TranslateUnit>& InUnit);
private slots:

    
private:
    std::unordered_set<std::shared_ptr<TranslateUnit>> translate_units;
};



#endif //TRANSLATEMANAGER_H



/*
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    TranslateManager translator;
    translator.translateText("Hello, how are you?", "English", "Korean");

    return a.exec();
}

#include "main.moc"*/
