// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_HISTORYMANAGER_H
#define FINTRANSLATOR_HISTORYMANAGER_H


#include <QObject>

#include "AbstractManager.h"

class FinTranslatorCore;

class HistoryManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit HistoryManager(FinTranslatorCore* parent);

    bool findTranslate();
    void addTranslate(const QString& inTr);
};


#endif //FINTRANSLATOR_HISTORYMANAGER_H
