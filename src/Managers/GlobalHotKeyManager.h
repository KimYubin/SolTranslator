// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H

#include "AbstractManager.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

#include <QHotkey>

class QHotkey;

namespace Sol
{
enum class Action;
class SolTranslatorCore;

/**
 * manage global hotkeys that can be detected, including the background.
 */
class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

    using ActionKeyHash = std::unordered_map<Action, std::unique_ptr<QHotkey>>;

public:
    explicit GlobalHotKeyManager(SolTranslatorCore* inParent);

    /**
     * Register the shortcut assigned to Action as a global shortcut.
     */
    void registerAction(const Action inActionType
                      , const QObject* inContext
                      , Callback<void(void)>&& inFunction);

    /**
     * Register global shortcut.
     */
    void registerHotKey(const Action inActionType
                      , const QKeySequence& inKeySeq
                      , const QObject* inContext
                      , Callback<void(void)>&& inFunction);


    Expected<void> changeHotkey(const Action inActionType
                              , const QKeySequence& inKeySeq);

    Expected<void> removeHotkey(const Action inActionType);

private:
    ActionKeyHash _hotKeys;
};


} // namespace Sol

#endif //GLOBALHOTKEYMANAGER_H
