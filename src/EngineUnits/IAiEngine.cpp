// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "IAiEngine.h"

namespace Sol
{
IAiEngine::IAiEngine(const EngineId& inEngine)
    : ITranslateEngine(inEngine)
{}

IAiEngine::~IAiEngine()
{}
} // namespace Sol
