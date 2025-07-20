// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H

#include <QNetworkRequest>
#include <QObject>
#include <QPointer>

#include "FinTypes.h"


class QNetworkReply;
class TranslateManager;

/**
 * 각 번역 요청을 독립적으로 수행하는 번역 유닛입니다.
 * 번역 요청에 필요한 정보를 보관하고, 전처리, 후처리 과정을 전담합니다.
 * TranslateManager::executeNewTranslateUnit()함수로 사용합니다.
 */
class TranslateUnit : public QObject
{
    Q_OBJECT

public:
    explicit TranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                         , TranslateManager* parent);

    void executeTextTranslation();

protected:
    void get(const QNetworkRequest& request);
    void post(const QNetworkRequest& request, const QByteArray& data, const bool bIsStreaming);

private:
    void postProcess();

protected:
    virtual void requestTranslate() = 0;
    virtual void onReadyRead() = 0;

private:
signals:
    /** 완료된 번역문을 등록된 슬롯에 적용합니다. */
    void onCompletedTranslate(const QString& TranslateText);

    /**  */
    void addStreamTranslatedText(const QString& TranslateText);

private slots:
    void onReplyFinished();

public slots:
    void abortTranslate();

protected:
    /** 받은 응답에서 번역문을 추출합니다. */
    virtual void replyTranslateFinished() = 0;

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
    QPointer<QNetworkReply> _reply;

    TranslateRequestInfo _trReqData;

    QString _translatedText;
};


#endif //TRANSLATEUNIT_H
