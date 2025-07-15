// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H
#include <QObject>

#include "AbstractManager.h"


enum class HotkeyType;
class QHotkey;
class FinTranslatorCore;

class GlobalHotKeyManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit GlobalHotKeyManager(FinTranslatorCore* parent);

private slots:
    void RegisterHotKey(HotkeyType InHotkey, const QKeySequence& shortcut, std::function<void(GlobalHotKeyManager*)> InFunction);

public:
    void FireSimpleTranslate();

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
