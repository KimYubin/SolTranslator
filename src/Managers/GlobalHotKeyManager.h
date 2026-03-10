// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H
#include <QObject>

#include <expected>

#include "AbstractManager.h"


enum class ShortCutType;
class QHotkey;
class SolTranslatorCore;

class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit GlobalHotKeyManager(SolTranslatorCore* parent);

private:
    void registerHotKey(ShortCutType inHotkey, const QKeySequence& inShortcut, const QObject* inContext, std::move_only_function<void(void)>&& inFunction);

    std::expected<void, QString> changeShortcut(ShortCutType inHotkey, const QKeySequence& shortcut);

public:
    void fireSimpleTranslate();

private:
    std::unordered_map<ShortCutType, QHotkey*> hotKeys;
};



#endif //GLOBALHOTKEYMANAGER_H
