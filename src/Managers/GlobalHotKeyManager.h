// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H
#include <QObject>

#include <expected>

#include "AbstractManager.h"


enum class HotkeyType;
class QHotkey;
class SolTranslatorCore;

class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit GlobalHotKeyManager(SolTranslatorCore* parent);

private:
    void registerHotKey(HotkeyType InHotkey, const QKeySequence& shortcut, QObject* inContext, std::function<void(void)>&& InFunction);
    std::expected<void, QString> changeShortcut(HotkeyType InHotkey, const QKeySequence& shortcut);

public:
    void fireSimpleTranslate();

private:
    std::unordered_map<HotkeyType, QHotkey*> hotKeys;
};

enum class HotkeyType
{
    None

  , SimpleTranslate

  , Size
};


#endif //GLOBALHOTKEYMANAGER_H
