// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include "ConfigManager.h"
#include "EngineManager.h"
#include "GlobalHotKeyManager.h"
#include "HistoryManager.h"
#include "InputSimulator.h"
#include "SolTranslatorCore.h"
#include "EngineUnits/TranslateUnit.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"
#include "Types/SolTypes.h"
#include "Types/TranslateRequest.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolAsync.hpp"
#include "Utils/SolLog.h"
#include "Widgets/PopupTranslateWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QTextDocument>

#include <magic_enum.hpp>


TranslateManager::TranslateManager(SolTranslatorCore* inParent): AbstractManager(inParent)
{
    _networkAccessManager = new QNetworkAccessManager(this);
}

void TranslateManager::init(HistoryManager* inHistoryManager
    , GlobalHotKeyManager* inGlobalHotKeyManager)
{
    Q_ASSERT_X(_historyManager.isNull(), "TranslateManager::init", "The _historyManager has been already initialized.");
    Q_ASSERT_X(_globalHotKeyManager.isNull(), "TranslateManager::init", "The _globalHotKeyManager has been already initialized.");

    _historyManager = inHistoryManager;
    _globalHotKeyManager = inGlobalHotKeyManager;
}

void TranslateManager::postInitialize()
{
    Q_ASSERT_X(_historyManager, "TranslateManager::postInitialize", "The _historyManager is not initialized.");
    Q_ASSERT_X(_globalHotKeyManager, "TranslateManager::postInitialize", "The _globalHotKeyManager is not initialized.");

    // global popup translate
    _globalHotKeyManager->registerAction(
        Action::PopupTranslate
      , this
      , [this]() { processPopupTranslate(); }
    );

    AbstractManager::postInitialize();
}

QNetworkReply* TranslateManager::get(const QNetworkRequest& inRequest)
{
    return _networkAccessManager->get(inRequest);
}

QNetworkReply* TranslateManager::post(const QNetworkRequest& inRequest, const QByteArray& inPayload)
{
    return _networkAccessManager->post(inRequest, inPayload);
}

Expected<TranslateUnit*> TranslateManager::newTranslateUnit(const EngineId& inEngineId)
{
    return EngineManager::newTrUnit(inEngineId, this);
}

Expected<QPointer<TranslateUnit>> TranslateManager::executeNewTranslateUnit(TranslateRequest&& inTrRequest)
{
    Q_ASSERT_X(_historyManager, "TranslateManager::executeNewTranslateUnit", "The _historyManager is not initialized.");

    // 앞뒤 공백 제거
    inTrRequest.sourceText = inTrRequest.sourceText.trimmed();

    const EngineId engineId   = inTrRequest.engineId;
    const QString sourceText  = inTrRequest.sourceText;
    const LangType sourceLang = inTrRequest.sourceLang;
    const LangType targetLang = inTrRequest.targetLang;
    const bool isIgnoreCache  = inTrRequest.isIgnoreCache;

    const Expected<TranslateUnit*> trUnitExp = newTranslateUnit(engineId);
    if (!trUnitExp)
    {
        return makeUnexpected(trUnitExp.error());
    }

    TranslateUnit* trUnit = trUnitExp.value();

    trUnit->setTranslateRequest(std::move(inTrRequest));

    if (sourceText.isEmpty())
    {
        solDebug << "translate request text is empty";

        trUnit->onTranslationFromCache("");
        return trUnit;
    }

    if (isIgnoreCache)
    {
        trUnit->requestTranslate();
        return trUnit;
    }

    _historyManager->asyncLookupHistory(
        engineId
      , sourceText
      , sourceLang
      , targetLang
      , trUnit
      , [inTrUnit = QPointer{trUnit}](const std::tuple<bool, QString>& inRes)
        {
            if (inTrUnit.isNull())
            {
                solDebug << "The trUnit was destroyed before the database lookup was completed.";
                return;
            }

            auto& [isFind, findCache] = inRes;
            if (isFind)
            {
                inTrUnit->onTranslationFromCache(findCache);
            }
            else
            {
                inTrUnit->requestTranslate();
            }
        });

    return trUnit;
}

Expected<QPointer<TranslateUnit>> TranslateManager::translateText(TranslateRequest&& inTrRequest)
{
    return executeNewTranslateUnit(std::move(inTrRequest));
}

void TranslateManager::translateAtPopup(const QString& inSourceText
                                      , const TextStyle inTextStyle
                                      , const bool inIsIgnoreCache)
{
    if (inSourceText.isEmpty())
    {
        return;
    }

    PopupTranslateWidget* popupWidget = new PopupTranslateWidget();
    popupWidget->executeTranslate(inSourceText, inTextStyle, LangType::AUTO, solConfig.popupTargetLang(), inIsIgnoreCache);
}

void TranslateManager::onAddHistoryRequested(const TranslateRequest& inTrRequest
                                           , const QString& inTargetText)
{
    if (inTargetText.isEmpty())
    {
        return;
    }

    _historyManager->asyncAddHistory(
        inTrRequest.engineId
      , inTrRequest.sourceLang
      , inTrRequest.targetLang
      , inTrRequest.sourceText
      , inTargetText
      , inTrRequest.textFormat
    );
}

void TranslateManager::processPopupTranslate()
{
    const QMimeData* prevClipboard = QApplication::clipboard()->mimeData();
    const QStringList formatsList = prevClipboard->formats();

    std::unique_ptr<QMimeData> prevMime = std::make_unique<QMimeData>();

    for (const QString& prevFormat : formatsList)
    {
        prevMime->setData(prevFormat, prevClipboard->data(prevFormat));
    }

    // 클립보드 갱신(복사) 대기
    QMetaObject::Connection clipboardConnection
        = connect(QApplication::clipboard(), &QClipboard::changed, this, [this, prevMimeChanged = std::move(prevMime)](const QClipboard::Mode inMode) mutable
    {
        const QMimeData* selectedMime = QApplication::clipboard()->mimeData(inMode);

        if (selectedMime == nullptr || selectedMime->hasText() == false)
        {
            return;
        }

        switch (inMode)
        {
        case QClipboard::Clipboard:
        {
            // 번역 실행
            if (selectedMime->hasHtml())
            {
                translateAtPopup(selectedMime->html(), TextStyle::Html);
            }
            else
            {
                translateAtPopup(selectedMime->text(), TextStyle::PlainText);
            }

            // 이전 클립보드 원상복구. 클립보드 clear() 대기
            if (prevMimeChanged->text() == selectedMime->text())
            {
                break;
            }

            connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [this, prevMimeDataChanged = std::move(prevMimeChanged)]() mutable
            {
                QTimer::singleShot(100, this, [prevMimeTimer = std::move(prevMimeDataChanged)]()
                {
                    QMimeData* copyMimeData = new QMimeData;

                    const QStringList prevMimeformats = prevMimeTimer->formats();
                    for (const QString& prevFormat : prevMimeformats)
                    {
                        copyMimeData->setData(prevFormat, prevMimeTimer->data(prevFormat));
                    }
                    // Transfer ownership
                    QApplication::clipboard()->setMimeData(copyMimeData);
                });
            }, Qt::SingleShotConnection);

            QApplication::clipboard()->clear();

            break;
        }
        case QClipboard::Selection: break;
        case QClipboard::FindBuffer: break;
        default: ;
        }
    }, Qt::SingleShotConnection);

    // Connection waiting time limit.
    // Prevents empty copies and unlimited waiting.
    QTimer::singleShot(500, this, [clipboardConnection]()
    {
        disconnect(clipboardConnection);
    });

    // execute copy
    InputSimulator::triggerCopy();
}


