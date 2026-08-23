// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "GlobalHotKeyManager.h"

#include "ConfigManager.h"
#include "SolTranslatorCore.h"
#include "TranslateManager.h"
#include "Types/SolTypes.h"
#include "Utils/EnumUtils.hpp"
#include "Utils/SolDebug.h"

#include <QHotkey>
#include <QKeyEvent>
#include <QThread>
#include <QTimer>

namespace Sol
{
GlobalHotKeyManager::GlobalHotKeyManager(SolTranslatorCore* inParent)
    : AbstractManager(inParent)
{}

void GlobalHotKeyManager::registerAction(const Action inActionType
                                       , const QObject* inContext
                                       , Callback<void(void)>&& inFunction)
{
    registerHotKey(inActionType
                 , solConfig.shortcut(inActionType)
                 , inContext
                 , std::move(inFunction));
}

void GlobalHotKeyManager::registerHotKey(const Action inActionType
                                       , const QKeySequence& inKeySeq
                                       , const QObject* inContext
                                       , Callback<void(void)>&& inFunction)
{
    if (_hotKeys.contains(inActionType))
    {
        solDebug << "The Action that already exists has been re-register.";
    }

    _hotKeys[inActionType] = std::make_unique<QHotkey>(inKeySeq, true);
    const QHotkey* hotkey  = _hotKeys[inActionType].get();

    connect(hotkey, &QHotkey::activated, inContext, std::move(inFunction));
}

Expected<void> GlobalHotKeyManager::changeHotkey(const Action inActionType
                                               , const QKeySequence& inKeySeq)
{
    const ActionKeyHash::iterator findIt = _hotKeys.find(inActionType);
    if (findIt == _hotKeys.end())
    {
        return makeUnexpected("Attempt to change non-existent shortcut: " + enumToQStr(inActionType) + inKeySeq.toString());
    }

    findIt->second->setShortcut(inKeySeq, true);

    return {};
}

Expected<void> GlobalHotKeyManager::removeHotkey(const Action inActionType)
{
    const ActionKeyHash::iterator findIt = _hotKeys.find(inActionType);
    if (findIt == _hotKeys.end())
    {
        return makeUnexpected("Attempt to remove non-existent shortcut: " + enumToQStr(inActionType));
    }

    _hotKeys.erase(findIt);

    return {};
}
} // namespace Sol
