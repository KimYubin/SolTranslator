// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H

#include "AbstractManager.h"

#include <expected>


class GlobalHotKeyManager;
enum class EngineType;
class HistoryManager;
class QNetworkRequest;
class QNetworkReply;
enum class TextStyle;
struct TranslateRequestInfo;
class QNetworkAccessManager;
class QTextEdit;
class TranslateUnit;
class SolTranslatorCore;
class QMimeData;


class TranslateManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit TranslateManager(SolTranslatorCore* parent);

    void init(HistoryManager* inHistoryManager
            , GlobalHotKeyManager* inGlobalHotKeyManager);

protected:
    virtual void postInitialize() override;

public:
    QNetworkReply* get(const QNetworkRequest& inRequest);
    QNetworkReply* post(const QNetworkRequest& inRequest, const QByteArray& inPayload);

private:
    TranslateUnit* newTranslateUnit(const EngineType inEngine);
    std::expected<QPointer<TranslateUnit>, QString> executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo);

public:
    std::expected<QPointer<TranslateUnit>, QString> translateText(TranslateRequestInfo&& inTranslateRequestInfo);

    void translateAtPopup(const QString& inSourceText
                        , const TextStyle inTextStyle
                        , const bool inIsIgnoreCache = false);

    void onAddHistoryRequested(const TranslateRequestInfo& inTranslateRequestInfo
                             , const QString& inTargetText);

private:
    void processPopupTranslate();

    QNetworkAccessManager* _networkAccessManager;

    QPointer<HistoryManager> _historyManager;
    QPointer<GlobalHotKeyManager> _globalHotKeyManager;
};


#endif //TRANSLATEMANAGER_H
