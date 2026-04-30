// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "IAiEngine.h"

IAiEngine::IAiEngine(const EngineId& inEngine)
    : ITranslateEngine(inEngine)
    , _defaultTemperature(0.5)
{}

IAiEngine::~IAiEngine()
{}

void IAiEngine::setDefaultPrompt(const QString& inDefaultPrompt)
{
    _defaultPrompt = inDefaultPrompt;
}

void IAiEngine::setDefaultTemperature(const double inDefaultTemperature)
{
    _defaultTemperature = inDefaultTemperature;
}

void IAiEngine::setDefaultModel(const QString& inDefaultModel)
{
    _defaultModel = inDefaultModel;
}
