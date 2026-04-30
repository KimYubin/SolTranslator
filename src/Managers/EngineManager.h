// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENGINEMANAGER_H
#define SOLTRANSLATOR_ENGINEMANAGER_H
#include "AbstractManager.h"

#include <expected>


class TranslateManager;
class EngineId;
class TranslateUnit;
class ITranslateEngine;

class EngineManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit EngineManager(SolTranslatorCore* parent);
    virtual ~EngineManager() override;

    static void registerEngine(ITranslateEngine* inEngine);
    static void unregisterEngine(ITranslateEngine* inEngine);

    static QPointer<ITranslateEngine> getEngine(const EngineId& inEngineId);

    static std::vector<QPointer<ITranslateEngine>> sortedTranslateEngineList();

    static std::expected<TranslateUnit*, QString> newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager);
};


#endif //SOLTRANSLATOR_ENGINEMANAGER_H
