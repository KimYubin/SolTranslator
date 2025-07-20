// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H

#include <QNetworkRequest>
#include <QObject>
#include <QPointer>

#include "FinTypes.h"


class QNetworkReply;
class TranslateManager;

class TranslateUnit : public QObject
{
    Q_OBJECT

public:
    explicit TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent);

    void executeTextTranslation();

protected:
    void get(const QNetworkRequest &request);
    void post(const QNetworkRequest& request, const QByteArray& data, const bool bIsStreaming);

private:
    void executeTextTranslation_Impl();

protected:
    virtual void requestTranslate() = 0;

    virtual void onReadyRead(QNetworkReply* reply);
private:
signals:
    /** 완료된 번역문을 등록된 슬롯에 적용합니다. */
    void onCompletedTranslate(const QString& TranslateText);

    /**  */
    void addStreamTranslatedText(const QString& TranslateText);

private slots:
    void onReplyFinished(/*QNetworkReply* reply*/);

public slots:
    void abortTranslate();
protected:
    /** 받은 응답에서 번역문을 추출합니다. */
    virtual void replyTranslateFinished(QNetworkReply* reply) = 0;

    /** 중간 번역을 반영합니다. */
    void addTranslatedText(const QString& inTranslatedText);

    /**
     * 추출 완료된 번역문에 대한 후처리를 합니다.
     * 캐시를 업데이트하고, 
     * 번역문을 등록된 signal을 발동하고, 본 객체를 파괴합니다.
     *
     * @see replyTranslate
     * @see ApplyCompletedTranslate
     * @param inTranslatedText
     */
    void completeTranslatedText(const QString& inTranslatedText);

protected:
    QPointer<TranslateManager> _translateManager;
    QPointer<QNetworkReply> _reply;

    TranslateRequestInfo _trReqData;

    QString _translatedText;
};


#endif //TRANSLATEUNIT_H
