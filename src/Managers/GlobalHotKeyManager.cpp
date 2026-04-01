// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "GlobalHotKeyManager.h"

#include "ConfigManager.h"
#include "SolTranslatorCore.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"
#include "TranslateManager.h"
#include "Utils/SolLog.h"

#include <QHotkey>
#include <QKeyEvent>
#include <QThread>
#include <QTimer>

GlobalHotKeyManager::GlobalHotKeyManager(SolTranslatorCore* parent)
    : AbstractManager(parent)
{}

void GlobalHotKeyManager::registerAction(const Action inActionType
                                       , const QObject* inContext
                                       , std::move_only_function<void(void)>&& inFunction)
{
    registerHotKey(inActionType
                 , solConfig.shortcut(inActionType)
                 , inContext
                 , std::move(inFunction));
}

void GlobalHotKeyManager::registerHotKey(const Action inActionType
                                       , const QKeySequence& inKeySeq
                                       , const QObject* inContext
                                       , std::move_only_function<void(void)>&& inFunction)
{
    if (_hotKeys.contains(inActionType))
    {
        solDebug << "The Action that already exists has been re-register.";
    }

    _hotKeys[inActionType] = std::make_unique<QHotkey>(inKeySeq, true);
    const QHotkey* hotkey  = _hotKeys[inActionType].get();

    connect(hotkey, &QHotkey::activated, inContext, std::move(inFunction));
}

std::expected<void, QString> GlobalHotKeyManager::changeHotkey(const Action inActionType
                                                             , const QKeySequence& inKeySeq)
{
    const ActionKeyHash::iterator findIt = _hotKeys.find(inActionType);
    if (findIt == _hotKeys.end())
    {
        return std::unexpected("Attempt to change non-existent shortcut: " + Sol::enumToQStr(inActionType) + inKeySeq.toString());
    }

    findIt->second->setShortcut(inKeySeq, true);

    return {};
}

std::expected<void, QString> GlobalHotKeyManager::removeHotkey(const Action inActionType)
{
    const ActionKeyHash::iterator findIt = _hotKeys.find(inActionType);
    if (findIt == _hotKeys.end())
    {
        return std::unexpected("Attempt to remove non-existent shortcut: " + Sol::enumToQStr(inActionType));
    }

    _hotKeys.erase(findIt);

    return {};
}
