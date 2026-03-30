// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TranslateManager.h"

#include "AsyncManager.h"
#include "ConfigManager.h"
#include "GlobalHotKeyManager.h"
#include "InputSimulator.h"
#include "SolTranslatorCore.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"
#include "EngineUnits/TranslateUnit.h"
#include "EngineUnits/FinPoint/FinPointTrUnit.h"
#include "EngineUnits/GoogleEngine/GoogleTrUnit.h"
#include "EngineUnits/OpenAI/OpenAiTrUnit.h"
#include "Utils/SolLog.h"
#include "Widgets/PopupTranslateWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QTextDocument>


TranslateManager::TranslateManager(SolTranslatorCore* parent): AbstractManager(parent)
{
    _networkAccessManager = new QNetworkAccessManager(this);
}

void TranslateManager::postInitialize()
{
    // global popup translate
    solCore->globalHotKeyManager()->registerAction(Action::PopupTranslate
                                                 , this
                                                 , [this]() { processPopupTranslate(); });

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

std::expected<QPointer<TranslateUnit>, QString> TranslateManager::executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo)
{
    TranslateUnit* trUnit = nullptr;
    const EngineType currentEngine = inTranslateRequestInfo.engineType;
    switch (currentEngine)
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
    case EngineType::Size:
        break;
    }

    if (trUnit == nullptr)
    {
        return std::unexpected{"Failed to create trUnit. Current engine: " + Sol::enumToQStr(currentEngine)};
    }

#ifdef QT_DEBUG
{
    // string 기반 enum과 class 매칭 유효성 검사
    bool isValidEngineName = false;
    if (const char* className = trUnit ? trUnit->metaObject()->className() : "")
    {
        if (magic_enum::enum_name(currentEngine).find(className))
        {
            isValidEngineName = true;
        }
    }
    Q_ASSERT_X(isValidEngineName, "TranslateManager::executeNewTranslateUnit", "Invalid engine type");
    if (isValidEngineName == false)
    {
        trUnit->deleteLater();
        return std::unexpected{"Invalid engine type"};
    }
}
#endif

    std::expected<void, QString> execRes = trUnit->executeTextTranslation(std::move(inTranslateRequestInfo));
    if (execRes.has_value() == false)
    {
        trUnit->deleteLater();
        return std::unexpected{execRes.error()};
    }

    return trUnit;
}

std::expected<QPointer<TranslateUnit>, QString> TranslateManager::translateText(TranslateRequestInfo&& inTranslateRequestInfo)
{
    return executeNewTranslateUnit(std::move(inTranslateRequestInfo));
}

void TranslateManager::translateAtPopup(const QString& inOriginText
                                      , const TextStyle inTextStyle)
{
    if (inOriginText.isEmpty())
    {
        return;
    }

    PopupTranslateWidget* popupWidget = new PopupTranslateWidget();
    popupWidget->executeTranslate(inOriginText, inTextStyle, LangType::AUTO, solConfig.popupTargetLang());
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
        = connect(QApplication::clipboard(), &QClipboard::changed, this, [this, prevMimeChanged = std::move(prevMime)](QClipboard::Mode mode) mutable
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

                    const QStringList formatsList = prevMimeTimer->formats();
                    for (const QString& prevFormat : formatsList)
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


