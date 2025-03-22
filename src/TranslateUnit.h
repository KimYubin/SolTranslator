//
// Created by YubinKim on 25/03/10 월.
//

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H

#include <QNetworkAccessManager>
#include <QTextEdit>

#include "FinTypes.h"


class QTextEdit;
class TranslateManager;

class TranslateUnit : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent);

    template <typename Func>
    void executeTextTranslation(const typename QtPrivate::ContextTypeForFunctor<Func>::ContextType* inTextEditableObj
                     , Func&& slotfunctor)
    {
        connect(this, &TranslateUnit::ApplyCompletedTranslate, inTextEditableObj, std::forward<Func>(slotfunctor));
        executeTextTranslation_Impl();
    }
    
    void executeTextTranslation();

private:
    void executeTextTranslation_Impl();

protected:
    virtual void requestTranslate() = 0;

private:
signals:
    /** 완료된 번역문을 등록된 슬롯에 적용합니다. */
    void ApplyCompletedTranslate(const QString& TranslateText);

    /**  */
    void addStreamTranslatedText(const QString& TranslateText);

private slots:
    void onReplyFinished(QNetworkReply* reply);

protected:
    /** 받은 응답에서 번역문을 추출합니다. */
    virtual void replyTranslateFinished(QNetworkReply* reply) = 0;

    /**
     * 추출 완료된 번역문에 대한 후처리를 합니다.
     * 캐시를 업데이트하고, 
     * 번역문을 등록된 signal을 발동하고, 본 객체를 파괴합니다.
     *
     * @see replyTranslate
     * @see ApplyCompletedTranslate
     * @param inTranslatedText
     */
    void updateTranslatedText(const QString& inTranslatedText);

    void addTranslatedText(const QString& inTranslatedText);

protected:
    TranslateRequestInfo trRequestInfo;
    QString originText;
    LangType sourceLang;
    LangType targetLang;

    QString translatedText;
};


#endif //TRANSLATEUNIT_H
