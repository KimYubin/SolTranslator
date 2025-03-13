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

    void translateSimple(const QString& text
                       , const QString& sourceLang
                       , const QString& targetLang);

private slots:


};

#endif //TRANSLATEMANAGER_H

