//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H
#include <QNetworkAccessManager>
#include <QTextEdit>


class QTextEdit;
class TranslateManager;

class TranslateUnit : public QNetworkAccessManager, public std::enable_shared_from_this<TranslateUnit>
{
    Q_OBJECT
private:
    struct Private{ explicit Private() = default; };
    TranslateUnit() = default;
public:
    explicit TranslateUnit(Private inPrivate, QObject* parent = nullptr);

    static std::shared_ptr<TranslateUnit> create(QObject* parent = nullptr)
    {
        return std::make_shared<TranslateUnit>(Private(), parent);
    }
    
    void translateText(TranslateManager* inTranslateManager
                       , QTextEdit* inTextEditableObj
                       , const QString& text
                       , const QString& sourceLang
                       , const QString& targetLang);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    TranslateManager* parentTranslateManager;
    QTextEdit* textEditableObject;
    // std::weak_ptr<QTextEdit> textEditableObject;
};



#endif //TRANSLATEUNIT_H
