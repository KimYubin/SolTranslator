// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EngineSelector.h"

#include "SolTranslatorCore.h"
#include "EngineUnits/ITranslateEngine.h"
#include "Managers/ConfigManager.h"
#include "Managers/EngineManager.h"
#include "Types/SolTypes.h"
#include "Utils/SolLog.h"

EngineSelector::EngineSelector(QWidget* parent) : DropdownMenu(parent)
{
    setEditable(false);

    const EngineId curEngineId = solConfig.currentEngineId();
    int curIdx = 0;

    const std::vector<QPointer<ITranslateEngine>> trEngineList = EngineManager::sortedTranslateEngineList();
    for (const QPointer<ITranslateEngine>& trEngine : trEngineList)
    {
        addItem(trEngine->getIcon(), trEngine->getDisplayName(), trEngine->getEngineId().toString());
        if (curEngineId == trEngine->getEngineId())
        {
            curIdx = (count() - 1);
        }
    }

    setCurrentIndex(curIdx);
}

void EngineSelector::setCurrentIndexChanged(std::move_only_function<void(const int)>&& inFunctor)
{
    connect(this, &QComboBox::currentIndexChanged, this, std::move(inFunctor));
}
