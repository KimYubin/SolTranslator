// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H

#include "AbstractManager.h"
#include "Types/SolExpected.hpp"


class EngineId;
class GlobalHotKeyManager;
class HistoryManager;
class QNetworkRequest;
class QNetworkReply;
enum class TextStyle;
struct TranslateRequest;
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
    void postInitialize() override;

public:
    QNetworkReply* get(const QNetworkRequest& inRequest);
    QNetworkReply* post(const QNetworkRequest& inRequest, const QByteArray& inPayload);

private:
    Expected<TranslateUnit*> newTranslateUnit(const EngineId& inEngineId);
    Expected<QPointer<TranslateUnit>> executeNewTranslateUnit(TranslateRequest&& inTrRequest);

public:
    Expected<QPointer<TranslateUnit>> translateText(TranslateRequest&& inTrRequest);

    void translateAtPopup(const QString& inSourceText
                        , const TextStyle inTextStyle
                        , const bool inIsIgnoreCache = false);

    void onAddHistoryRequested(const TranslateRequest& inTrRequest
                             , const QString& inTargetText);

private:
    void processPopupTranslate();

    QNetworkAccessManager* _networkAccessManager;

    QPointer<HistoryManager> _historyManager;
    QPointer<GlobalHotKeyManager> _globalHotKeyManager;
};


#endif //TRANSLATEMANAGER_H
