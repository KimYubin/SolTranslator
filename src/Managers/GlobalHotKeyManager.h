// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H
#include <QObject>

#include <expected>

#include "AbstractManager.h"


enum class Action;
class QHotkey;
class SolTranslatorCore;

/**
 * manage global hotkeys that can be detected, including the background.
 */
class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit GlobalHotKeyManager(SolTranslatorCore* parent);

    /**
     * Register the shortcut assigned to Action as a global shortcut.
     */
    void registerAction(const Action inShortCutType
                      , const QObject* inContext
                      , std::move_only_function<void(void)>&& inFunction);

    /**
     * Register global shortcut.
     */
    void registerHotKey(const Action inShortCutType
                      , const QKeySequence& inKeySeq
                      , const QObject* inContext
                      , std::move_only_function<void(void)>&& inFunction);


    std::expected<void, QString> changeHotkey(const Action inShortCutType
                                            , const QKeySequence& inKeySeq);

    std::expected<void, QString> removeHotkey(const Action inShortCutType);

private:
    std::unordered_map<Action, QHotkey*> hotKeys;
};


#endif //GLOBALHOTKEYMANAGER_H
