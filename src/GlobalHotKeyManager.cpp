//
// Created by YubinKim on 25/03/13 목.
//

#include "GlobalHotKeyManager.h"

#include "qhotkey.h"

GlobalHotKeyManager::GlobalHotKeyManager(QObject* parent): QObject(parent)
{
    RegisterHotKey(HotkeyType::simpleTranslate, QKeySequence("Alt+Z"), GlobalHotKeyManager::FireSimpleTranslate);
}

void GlobalHotKeyManager::RegisterHotKey(HotkeyType InHotkey, const QKeySequence& shortcut, std::function<void(GlobalHotKeyManager*)> InFunction)
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
    QObject::connect(hotkey, &QHotkey::activated, this, std::move(InFunction));
}

void GlobalHotKeyManager::FireSimpleTranslate()
{
}
