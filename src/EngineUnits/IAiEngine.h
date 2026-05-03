// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_IAIENGINE_H
#define SOLTRANSLATOR_IAIENGINE_H
#include "ITranslateEngine.h"
#include "Types/OptionKey.h"


class AiTranslateUnit;

class IAiEngine : public ITranslateEngine
{
    Q_OBJECT

public:
    explicit IAiEngine(const EngineId& inEngine);
    virtual ~IAiEngine() override;

protected:
    template <std::derived_from<AiTranslateUnit> T>
    void setTrUnitCreatorHelper()
    {
        setTrUnitCreator([this](TranslateManager* inTrManager)
        {
            return new T{inTrManager, this};
        });
    }
};

namespace AiOptionKey
{
inline const OptionKey model{"model"};
inline const OptionKey prompt{"prompt"};
inline const OptionKey temperature{"temperature"};
}


#endif //SOLTRANSLATOR_IAIENGINE_H
