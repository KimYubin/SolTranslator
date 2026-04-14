// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNIT_H
#define TRANSLATEUNIT_H

#include "Types/SolTypes.h"

#include <QObject>
#include <QPointer>


class HistoryManager;
class QNetworkRequest;
class QNetworkReply;
class TranslateManager;

/**
 * The TranslateUnit class provides independent translation requests process.
 */
class TranslateUnit : public QObject
{
    Q_OBJECT

public:
    explicit TranslateUnit(TranslateManager* parent);

    void setTranslateRequestInfo(TranslateRequestInfo&& inTranslateRequestInfo);
    void onTranslationFromCache(const QString& inTargetText);

    virtual void requestTranslate() = 0;

protected:
    void get(const QNetworkRequest& inRequest);
    void post(const QNetworkRequest& inRequest, const QByteArray& inPayload, const bool inIsStreaming);

private:
    void postProcess();

protected:
    virtual void onReadyRead() = 0;

private slots:
    void onReplyFinished();

    void disconnectTranslateDisplay();

public:
    /**
     * 번역 결과를 출력하지 않게 변경합니다.
     * 번역 결과는 history에만 저장됩니다.
     */
    void detachDisplayWidget();

    void abortTranslateRequest();

protected:
    /** 받은 응답에서 번역문을 추출합니다. */
    virtual void replyTranslateFinished() = 0;

    virtual void replyFailed();

    /** 번역 스트리밍 중간 내용을 반영합니다. */
    void appendTranslatedText(const QString& inDeltaTargetText);

    /**
     * 영구 데이터로 기록합니다.
     *
     * @param inTargetText 
     */
    void addHistory(const QString& inTargetText);

    /** 번역문을 등록된 펑터에 반영하고, 본 객체를 파괴합니다.(deleteLater) */
    void completeTranslatedText(const QString& inTargetText);

    /**
     * DB 업데이트와 번역문 개시 처리를 합니다.
     * 본 객체를 파괴합니다.(deleteLater)
     *
     * @see replyTranslate
     * @see completeTranslatedText
     * @param inTargetText
     */
    void finishTranslateRequest(const QString& inTargetText);

protected:
    QPointer<TranslateManager> _translateManager;

    QPointer<QNetworkReply> _reply;

    TranslateRequestInfo _trReqData;

    // SSE buffer
    QByteArray _buffer;

    QString _targetText;

    QMetaObject::Connection _streamConnection;
    QMetaObject::Connection _completeConnection;

    bool _isStream = false;
};


#endif //TRANSLATEUNIT_H
