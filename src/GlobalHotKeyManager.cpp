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
    RegisterHotKey(HotkeyType::simpleTranslate, QKeySequence("Alt+Z"), &GlobalHotKeyManager::FireSimpleTranslate);
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
    QKeyEvent* pressEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier);
    QKeyEvent* releaseEvent = new QKeyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier);

    
    QGuiApplication::postEvent(QGuiApplication::focusWindow(), pressEvent);
    QGuiApplication::postEvent(QGuiApplication::focusWindow(), releaseEvent);
    RunCopKey::DoCopy();
    QThread::msleep(100); // 클립보드 갱신 대기

    // 2. 클립보드에서 텍스트 가져오기
    
    QClipboard *clipboard = QApplication::clipboard();
    QString selectedText = clipboard->text(/*QClipboard::Selection*/);

    if (FinTranslator* Fin = dynamic_cast<FinTranslator*>(parent()))
    {
        Fin->onSimpleTranslate(selectedText);
    }
}
