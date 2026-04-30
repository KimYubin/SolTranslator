// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "AiTranslateUnit.h"

#include "IAiEngine.h"

AiTranslateUnit::AiTranslateUnit(TranslateManager* parent, IAiEngine* inEngine)
    : TranslateUnit(parent, inEngine)
{}

AiTranslateUnit::~AiTranslateUnit()
{}

IAiEngine* AiTranslateUnit::aiEngine() const
{
    return qobject_cast<IAiEngine*>(_trEngine);
}
