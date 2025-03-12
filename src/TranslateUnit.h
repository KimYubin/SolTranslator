//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H
#include <QNetworkAccessManager>
#include <QTextEdit>


class QTextEdit;
class TranslateManager;

class TranslateUnit : public QNetworkAccessManager
{
    Q_OBJECT

public:
    TranslateUnit(QObject* parent);

    void translateText(QTextEdit* inTextEditableObj
                       , const QString& text
                       , const QString& sourceLang
                       , const QString& targetLang);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QTextEdit* textEditableObject;
    // std::weak_ptr<QTextEdit> textEditableObject;
};


#endif //TRANSLATEUNIT_H
