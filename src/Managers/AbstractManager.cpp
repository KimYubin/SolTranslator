// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "AbstractManager.h"

#include "SolTranslatorCore.h"

AbstractManager::AbstractManager(SolTranslatorCore* inParent)
    : QObject(nullptr) // Manage lifecycle with unique_ptr.
    , _solCore(inParent)
{
    Q_ASSERT_X(_solCore, "AbstractManager::AbstractManager", "SolTranslatorCore is invalid.");

    connect(_solCore, &SolTranslatorCore::postInitialized, this, &AbstractManager::postInitialize);
}

AbstractManager::~AbstractManager()
{}

void AbstractManager::postInitialize()
{}
