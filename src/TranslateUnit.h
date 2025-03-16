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
    TranslateUnit(TranslateManager* parent);

    template <typename Func>
    void translateText(const typename QtPrivate::ContextTypeForFunctor<Func>::ContextType* inTextEditableObj
                     , Func&& slotfunctor
                     , const QString& text
                     , const QString& sourceLang
                     , const QString& targetLang)
    {
        connect(this, &TranslateUnit::CompletedTranslate, inTextEditableObj, std::forward<Func>(slotfunctor));
        translateText_Impl(text, sourceLang, targetLang);
    }


    void translateText_Impl(const QString& inText
                          , const QString& inSourceLang
                          , const QString& inTargetLang);

signals:
    void CompletedTranslate(const QString& TranslateText);
    
private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QString originText;
    QString sourceLang;
    QString targetLang;
};


#endif //TRANSLATEUNIT_H
