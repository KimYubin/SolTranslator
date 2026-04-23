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
    virtual ~TranslateUnit() override;

    void setTranslateRequestInfo(TranslateRequestInfo&& inTranslateRequestInfo);
    void onTranslationFromCache(const QString& inTargetText);

    virtual void requestTranslate() = 0;

protected:
    void get(const QNetworkRequest& inRequest);
    void post(const QNetworkRequest& inRequest, const QByteArray& inPayload, const bool inIsStreaming);

private:
    void postProcess();

    void cleanUpReply();

protected:
    virtual void onReadyRead() = 0;

private slots:
    void onReplyFinished();
    void onReplyErrorOccurred(/*const QNetworkReply::NetworkError inNetworkError*/);
    void onReplyDestroyed();

    void disconnectTranslateDisplay();

public:
    /**
     * Disconnect from the display.
     * The translation result is only saved in history.
     */
    void detachDisplayWidget();

    void abortTranslateRequest();

protected:
    /** Extract the translation from the received response. */
    virtual QString replyTranslateFinished() = 0;

    QString replyErrorString() const;
    void replyFailed(const QString& inReason);

    /** Append the streaming results to the targetText. */
    void appendTranslatedText(const QString& inDeltaTargetText);

    /** update DB */
    void addHistory(const QString& inTargetText);

    /** Send the \a inTargetText to the display, This object will be destroyed. (deleteLater) */
    void completeTranslatedText(const QString& inTargetText);

    /**
     * Call addHistory() and completeTranslatedText()
     * 
     * @see replyTranslateFinished
     * @see addHistory
     * @see completeTranslatedText
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
    bool _isReplyFinished = false;
};


#endif //TRANSLATEUNIT_H
