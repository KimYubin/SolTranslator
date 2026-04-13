// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include "ConfigManager.h"
#include "GlobalHotKeyManager.h"
#include "HistoryManager.h"
#include "InputSimulator.h"
#include "SolTranslatorCore.h"
#include "EngineUnits/TranslateUnit.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"
#include "Types/SolTypes.h"
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


TranslateManager::TranslateManager(SolTranslatorCore* parent): AbstractManager(parent)
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

TranslateUnit* TranslateManager::newTranslateUnit(const EngineType inEngine)
{
    TranslateUnit* trUnit = nullptr;
    switch (inEngine)
    {
    case EngineType::Google:
        trUnit = new GoogleTrUnit(this);
        break;
    case EngineType::OpenAI:
        trUnit = new OpenAiTrUnit(this);
        break;
    case EngineType::FinPoint:
        trUnit = new FinPointTrUnit(this);
        break;
    case EngineType::FinPointDebug:
    {
        trUnit = new FinPointTrUnit(this);
        static_cast<FinPointTrUnit*>(trUnit)->setDebugMode(true);
        break;
    }

    case EngineType::Size: Q_UNREACHABLE();
        // default: Should not be used. There must be a 'case' for every enum class member.
    }

    Q_ASSERT_X(trUnit, "TranslateManager::newTranslateUnit", "trUnit is nullptr");

#ifdef QT_DEBUG
    {
        // string 기반 enum과 class 매칭 유효성 검사
        bool isValidEngineName = false;
        if (const char* className = trUnit->metaObject()->className())
        {
            if (magic_enum::enum_name(inEngine).find(className))
            {
                isValidEngineName = true;
            }
        }

        Q_ASSERT_X(isValidEngineName, "TranslateManager::newTranslateUnit", "Invalid engine type");
    }
#endif


    return trUnit;
}

std::expected<QPointer<TranslateUnit>, QString> TranslateManager::executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo)
{
    Q_ASSERT_X(_historyManager, "TranslateManager::executeNewTranslateUnit", "The _historyManager is not initialized.");

    // 앞뒤 공백 제거
    inTranslateRequestInfo.sourceText = inTranslateRequestInfo.sourceText.trimmed();

    const EngineType engineType = inTranslateRequestInfo.engineType;
    const QString sourceText    = inTranslateRequestInfo.sourceText;
    const LangType sourceLang   = inTranslateRequestInfo.sourceLang;
    const LangType targetLang   = inTranslateRequestInfo.targetLang;
    const bool isIgnoreCache    = inTranslateRequestInfo.isIgnoreCache;

    TranslateUnit* trUnit = newTranslateUnit(engineType);

    trUnit->setTranslateRequestInfo(std::move(inTranslateRequestInfo));

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
        engineType
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

std::expected<QPointer<TranslateUnit>, QString> TranslateManager::translateText(TranslateRequestInfo&& inTranslateRequestInfo)
{
    return executeNewTranslateUnit(std::move(inTranslateRequestInfo));
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

void TranslateManager::onAddHistoryRequested(const TranslateRequestInfo& inTranslateRequestInfo
                                           , const QString& inTargetText)
{
    if (inTargetText.isEmpty())
    {
        return;
    }

    _historyManager->asyncAddHistory(
        inTranslateRequestInfo.engineType
      , inTranslateRequestInfo.sourceLang
      , inTranslateRequestInfo.targetLang
      , inTranslateRequestInfo.sourceText
      , inTargetText
      , inTranslateRequestInfo.textFormat
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
        = connect(QApplication::clipboard(), &QClipboard::changed, this, [this, prevMimeChanged = std::move(prevMime)](const QClipboard::Mode mode) mutable
    {
        const QMimeData* selectedMime = QApplication::clipboard()->mimeData(mode);

        if (selectedMime == nullptr || selectedMime->hasText() == false)
        {
            return;
        }

        switch (mode)
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


