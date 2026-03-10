// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "AbstractManager.h"

#include "SolLog.h"
#include "SolTranslatorCore.h"

AbstractManager::AbstractManager(SolTranslatorCore* parent) : QObject(parent)
{}

void AbstractManager::postInitialize()
{}

