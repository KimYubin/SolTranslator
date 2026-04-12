// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "AbstractManager.h"

#include "SolTranslatorCore.h"

AbstractManager::AbstractManager(SolTranslatorCore* parent) : QObject(parent)
{
    connect(parent, &SolTranslatorCore::postInitialized, this, &AbstractManager::postInitialize);
}

void AbstractManager::postInitialize()
{}

