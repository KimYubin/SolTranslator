// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "GlobalHotKeyManager.h"

#include <QClipboard>
#include <QKeyEvent>
#include <QThread>
#include <QTimer>

#include <QHotkey>

#include "ConfigManager.h"
#include "SolUtilibrary.h"
#include "TranslateManager.h"
#include "SolTypes.h"

GlobalHotKeyManager::GlobalHotKeyManager(SolTranslatorCore* parent)
    : AbstractManager(parent)
{}

void GlobalHotKeyManager::registerAction(const Action inShortCutType
                                       , const QObject* inContext
                                       , std::move_only_function<void()>&& inFunction)
{
    registerHotKey(inShortCutType
                 , solConfig.shortcut(inShortCutType)
                 , inContext
                 , std::move(inFunction));
}

void GlobalHotKeyManager::registerHotKey(const Action inShortCutType
                                       , const QKeySequence& inKeySeq
                                       , const QObject* inContext
                                       , std::move_only_function<void()>&& inFunction)
{
    std::unordered_map<Action, QHotkey*>::iterator findIt = hotKeys.find(inShortCutType);

    QHotkey* hotkey;
    if (findIt == hotKeys.end())
    {
        hotkey = new QHotkey{inKeySeq, true, this};
        hotKeys[inShortCutType] = hotkey;
    }
    else
    {
        hotkey = findIt->second;
        hotkey->setShortcut(inKeySeq, true);
    }

    connect(hotkey, &QHotkey::activated, inContext, std::move(inFunction));
}

std::expected<void, QString> GlobalHotKeyManager::changeHotkey(const Action inShortCutType, const QKeySequence& inKeySeq)
{
    std::unordered_map<Action, QHotkey*>::iterator findIt = hotKeys.find(inShortCutType);
    if (findIt == hotKeys.end())
    {
        return std::unexpected("not found registered hotkeys: " + sol::enumToQStr(inShortCutType) + inKeySeq.toString());
    }

    findIt->second->setShortcut(inKeySeq, true);

    return {};
}

std::expected<void, QString> GlobalHotKeyManager::removeHotkey(const Action inShortCutType)
{
    std::unordered_map<Action, QHotkey*>::iterator findIt = hotKeys.find(inShortCutType);
    if (findIt == hotKeys.end())
    {
        return std::unexpected("not existent shortcut remove: " + sol::enumToQStr(inShortCutType));
    }

    findIt->second->deleteLater();
    hotKeys.erase(findIt);

    return {};
}
