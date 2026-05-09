// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_IAIENGINE_H
#define SOLTRANSLATOR_IAIENGINE_H
#include "ITranslateEngine.h"
#include "Types/OptionKey.h"


class IAiEngine : public ITranslateEngine
{
    Q_OBJECT

public:
    explicit IAiEngine(const EngineId& inEngine);
    virtual ~IAiEngine() override;
};

namespace AiOptionKey
{
inline const OptionKey Model{"model"};
inline const OptionKey Prompt{"prompt"};
inline const OptionKey Temperature{"temperature"};
}


#endif //SOLTRANSLATOR_IAIENGINE_H
