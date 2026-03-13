// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H

#include <expected>
#include <QHotkey>

#include "AbstractManager.h"
#include "SolTypes.h"


enum class Action;
class QHotkey;
class SolTranslatorCore;

/**
 * manage global hotkeys that can be detected, including the background.
 */
class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

    using ActionKeyHash = std::unordered_map<Action, std::unique_ptr<QHotkey>>;

public:
    explicit GlobalHotKeyManager(SolTranslatorCore* parent);

    /**
     * Register the shortcut assigned to Action as a global shortcut.
     */
    void registerAction(const Action inActionType
                      , const QObject* inContext
                      , std::move_only_function<void(void)>&& inFunction);

    /**
     * Register global shortcut.
     */
    void registerHotKey(const Action inActionType
                      , const QKeySequence& inKeySeq
                      , const QObject* inContext
                      , std::move_only_function<void(void)>&& inFunction);


    std::expected<void, QString> changeHotkey(const Action inActionType
                                            , const QKeySequence& inKeySeq);

    std::expected<void, QString> removeHotkey(const Action inActionType);

private:
    ActionKeyHash _hotKeys;
};


#endif //GLOBALHOTKEYMANAGER_H
