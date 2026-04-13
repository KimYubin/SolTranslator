// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITGOOGLE_H
#define TRANSLATEUNITGOOGLE_H

#include "EngineUnits/TranslateUnit.h"


class GoogleTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit GoogleTrUnit(TranslateManager* parent);

    virtual void requestTranslate() override;

protected:
    virtual void onReadyRead() override;
    virtual void replyTranslateFinished() override;
};


#endif //TRANSLATEUNITGOOGLE_H
