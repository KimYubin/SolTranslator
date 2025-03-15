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

#include "fintranslator.h"
#include "RunCopKey.h"

GlobalHotKeyManager::GlobalHotKeyManager(QObject* parent): QObject(parent)
{
    RegisterHotKey(HotkeyType::simpleTranslate, QKeySequence("Alt+X"), &GlobalHotKeyManager::FireSimpleTranslate);
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
    QObject::connect(hotkey, &QHotkey::activated, this, std::bind(InFunction, this));
}

void GlobalHotKeyManager::FireSimpleTranslate()
{
    QClipboard* clipboard = QApplication::clipboard();

    QString lastClipboardText = clipboard->text();

    RunCopKey::DoCopy();

    // 클립보드 갱신 대기
    connect(clipboard, &QClipboard::changed, clipboard, [&, inLastText = std::move(lastClipboardText)](QClipboard::Mode mode)
    {
        switch (mode)
        {
        case QClipboard::Clipboard:
        {
            const QString selectedText = QApplication::clipboard()->text();
            if (FinTranslator* Fin = dynamic_cast<FinTranslator*>(parent()))
            {
                Fin->onSimpleTranslate(selectedText);
            }

            // 이전 클립보드 원상복구
            if (inLastText != selectedText)
            {
                clipboard->clear();
            }
            break;
        }
        case QClipboard::Selection: break;
        case QClipboard::FindBuffer: break;
        default: ;
        }
    }, Qt::SingleShotConnection);
}
