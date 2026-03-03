// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GlobalHotKeyManager.h"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMimeData>
#include <QRegularExpression>
#include <QTextDocument>
#include <QThread>
#include <QTimer>
#include <QWindow>

#include <QHotkey>

#include "SolTranslatorCore.h"
#include "InputSimulator.h"
#include "SolUtilibrary.h"
#include "TranslateManager.h"

GlobalHotKeyManager::GlobalHotKeyManager(SolTranslatorCore* parent) : AbstractManager(parent)
{
    registerHotKey(HotkeyType::SimpleTranslate, QKeySequence(Qt::ALT | Qt::Key_C), this, [this]() { fireSimpleTranslate(); });
}

void GlobalHotKeyManager::registerHotKey(const HotkeyType inHotkey
                                       , const QKeySequence& inShortcut
                                       , const QObject* inContext
                                       , std::move_only_function<void()>&& inFunction)
{
    std::unordered_map<HotkeyType, QHotkey*>::iterator findIt = hotKeys.find(inHotkey);

    QHotkey* hotkey;
    if (findIt == hotKeys.end())
    {
        hotkey = new QHotkey{inShortcut, true, this};
        hotKeys[inHotkey] = hotkey;
    }
    else
    {
        hotkey = findIt->second;
        hotkey->setShortcut(inShortcut, true);
    }

    connect(hotkey, &QHotkey::activated, inContext, std::move(inFunction));
}

std::expected<void, QString> GlobalHotKeyManager::changeShortcut(HotkeyType inHotkey, const QKeySequence& shortcut)
{
    std::unordered_map<HotkeyType, QHotkey*>::iterator findIt = hotKeys.find(inHotkey);
    if (findIt == hotKeys.end())
    {
        return std::unexpected("not found registered hotkeys:" + sol::enumToQStr(inHotkey) + shortcut.toString());
    }

    findIt->second->setShortcut(shortcut, true);

    return {};
}

void GlobalHotKeyManager::fireSimpleTranslate()
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
                solCore->translateManager()->translateAtPopup(selectedMime->html(), TextStyle::Html);
            }
            else
            {
                solCore->translateManager()->translateAtPopup(selectedMime->text(), TextStyle::PlainText);
            }

            // 이전 클립보드 원상복구. 클립보드 clear() 대기
            if (prevMimeChanged->text() == selectedMime->text())
            {
                break;
            }

            connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [this, prevMimeDataChanged = std::move(prevMimeChanged)]() mutable
            {
                QTimer::singleShot(100, this, [this, prevMimeTimer = std::move(prevMimeDataChanged)]()
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

    // 연결 대기 시간 제한.
    // 비어있는 복사와 무제한 대기를 방지합니다.
    QTimer::singleShot(500, this, [clipboardConnection]()
    {
        disconnect(clipboardConnection);
    });

    // 복사 실행
    InputSimulator::triggerCopy();
}
