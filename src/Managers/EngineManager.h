// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENGINEMANAGER_H
#define SOLTRANSLATOR_ENGINEMANAGER_H

#include "AbstractManager.h"
#include "Types/SolExpected.hpp"


class TranslateManager;
class EngineId;
class TranslateUnit;
class ITranslateEngine;

class EngineManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit EngineManager(SolTranslatorCore* inParent);
    ~EngineManager() override;

protected:
    void postInitialize() override;

public:
    static void registerEngine(ITranslateEngine* inEngine);
    static void unregisterEngine(ITranslateEngine* inEngine);

    static QPointer<ITranslateEngine> getEngine(const EngineId& inEngineId);

    static std::vector<QPointer<ITranslateEngine>> sortedTranslateEngineList();

    /**
     * Return all TranslateEngine that can be cast to type T.
     * The list is sorted.
     */
    template <std::derived_from<ITranslateEngine> T>
    static std::vector<QPointer<T>> findEngines();

    static Expected<TranslateUnit*> newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager);
};


template <std::derived_from<ITranslateEngine> T>
std::vector<QPointer<T>> EngineManager::findEngines()
{
    std::vector<QPointer<ITranslateEngine>> allList = sortedTranslateEngineList();
    std::vector<QPointer<T>> resVec;
    resVec.reserve(allList.size());

    for (QPointer<ITranslateEngine>& trEngine : allList)
    {
        if (T* castObj = qobject_cast<T*>(trEngine))
        {
            resVec.emplace_back(castObj);
        }
    }

    return resVec;
}


#endif //SOLTRANSLATOR_ENGINEMANAGER_H
