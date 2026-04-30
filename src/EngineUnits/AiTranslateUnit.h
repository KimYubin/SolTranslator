// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_AITRUNIT_H
#define SOLTRANSLATOR_AITRUNIT_H
#include "TranslateUnit.h"


class IAiEngine;

class AiTranslateUnit : public TranslateUnit
{
    Q_OBJECT

public:
    explicit AiTranslateUnit(TranslateManager* parent, IAiEngine* inEngine);
    virtual ~AiTranslateUnit() override;

protected:
    IAiEngine* aiEngine() const;
};


#endif //SOLTRANSLATOR_AITRUNIT_H
