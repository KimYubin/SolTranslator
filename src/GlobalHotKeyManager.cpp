//
// Created by YubinKim on 25/03/13 목.
//

#include "GlobalHotKeyManager.h"

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QWindow>

#include "qhotkey.h"
#include <QClipboard>
#include <QThread>
#include <QMimeData>
#include <qregularexpression.h>
#include <qtextdocument.h>
#include <QTimer>

#include "FinTranslatorCore.h"
#include "RunCopKey.h"

GlobalHotKeyManager::GlobalHotKeyManager(FinTranslatorCore* parent): AbstractManager(parent)
{
    RegisterHotKey(HotkeyType::SimpleTranslate, QKeySequence("Alt+C"), &GlobalHotKeyManager::FireSimpleTranslate);
}

void GlobalHotKeyManager::RegisterHotKey(const HotkeyType InHotkey, const QKeySequence& shortcut, std::function<void(GlobalHotKeyManager*)> InFunction)
{
    std::unordered_map<HotkeyType, QHotkey*>::iterator findIt = hotKeys.find(InHotkey);

    QHotkey* hotkey;
    if (findIt == hotKeys.end())
    {
        hotkey            = new QHotkey{shortcut, true, this};
        hotKeys[InHotkey] = hotkey;
    }
    else
    {
        hotkey = findIt->second;
        hotkey->setShortcut(shortcut, true);
    }

    connect(hotkey, &QHotkey::activated, this, std::bind(InFunction, this));
}

void GlobalHotKeyManager::FireSimpleTranslate()
{
    const QMimeData* prevMime = QApplication::clipboard()->mimeData();
    QStringList formatsList   = prevMime->formats();

    std::unique_ptr<QMimeData> prevMimePtr = std::make_unique<QMimeData>();

    for (QString& prevFormat : formatsList)
    {
        prevMimePtr->setData(std::move(prevFormat), prevMime->data(prevFormat));
    }

    // 클립보드 갱신(복사) 대기
    connect(QApplication::clipboard(), &QClipboard::changed, this, [this, prevMimePtrChanged = std::move(prevMimePtr)](QClipboard::Mode mode) mutable
    {
        const QMimeData* selectedMime  = QApplication::clipboard()->mimeData(mode);

        if (selectedMime->hasText() == false)
        {
            return;
        }

        switch (mode)
        {
        case QClipboard::Clipboard:
        {
            // 번역 실행
            if (FinTranslatorCore* Fin = getFinCore())
            {
                Fin->onSimpleTranslate(selectedMime);
            }

            if (prevMimePtrChanged->text() == selectedMime->text())
            {
                break;
            }

            // 이전 클립보드 원상복구. 클립보드 clear() 대기
            connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [this, prevMimePtrDataChanged = std::move(prevMimePtrChanged)]() mutable
            {
                // 잠시 대기 후 원복
                QTimer::singleShot(100, this, [this, prevMimePtrTimer = std::move(prevMimePtrDataChanged)]()
                {
                    QMimeData* copyMimeData = new QMimeData;

                    QStringList formatsList = prevMimePtrTimer->formats();
                    for (QString& prevFormat : formatsList)
                    {
                        copyMimeData->setData(std::move(prevFormat), prevMimePtrTimer->data(prevFormat));
                    }

                    QApplication::clipboard()->setMimeData(copyMimeData);
                });
            }, Qt::SingleShotConnection);

            // 번역에 이용한 클립보드 내용 제거.
            QApplication::clipboard()->clear();

            break;
        }
        case QClipboard::Selection: break;
        case QClipboard::FindBuffer: break;
        default: ;
        }
    }, Qt::SingleShotConnection);

    // 복사 실행
    RunCopKey::DoCopy();
}
