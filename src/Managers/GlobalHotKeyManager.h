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

    void registerHotKey(const ShortCutType inShortCutType
                      , const QKeySequence& inKeySeq
                      , const QObject* inContext
                      , std::move_only_function<void(void)>&& inFunction);

    std::expected<void, QString> changeHotkey(const ShortCutType inShortCutType
                                            , const QKeySequence& inKeySeq);

    std::expected<void, QString> removeHotkey(const ShortCutType inShortCutType);

private:
    std::unordered_map<ShortCutType, QHotkey*> hotKeys;
};


#endif //GLOBALHOTKEYMANAGER_H
