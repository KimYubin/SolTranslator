// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "AbstractManager.h"

#include "SolTranslatorCore.h"

AbstractManager::AbstractManager(SolTranslatorCore* parent) : QObject(parent)
{
    connect(solCore, &SolTranslatorCore::postInitialized, this, &AbstractManager::postInitialize);
}

void AbstractManager::postInitialize()
{}

