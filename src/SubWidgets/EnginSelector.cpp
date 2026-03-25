// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "EnginSelector.h"

#include "SolTranslatorCore.h"
#include "SolTypes.h"

#include "Managers/ConfigManager.h"

EnginSelector::EnginSelector(QWidget* parent) : DropdownMenu(parent)
{
    setEditable(false);

    // 엔진 선택 초기화
    for (EngineType eg = EngineType::Google; eg != EngineType::Size; eg = static_cast<EngineType>(static_cast<int>(eg) + 1))
    {
        addItem(EngineHelper::displayName(eg), static_cast<int>(eg));
    }

    setCurrentIndex(static_cast<int>(solConfig.currentEngineType()));
}

void EnginSelector::setCurrentIndexChanged(std::move_only_function<void(const int)>&& inFunctor)
{
    connect(this, &QComboBox::currentIndexChanged, this, std::move(inFunctor));
}
