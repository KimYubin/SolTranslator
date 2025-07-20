// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITGOOGLE_H
#define TRANSLATEUNITGOOGLE_H

#include "EngineUnits/TranslateUnit.h"


class GoogleTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit GoogleTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                        , TranslateManager* parent);

protected:
    virtual void requestTranslate() override;
    virtual void onReadyRead() override;
    virtual void replyTranslateFinished() override;
};


#endif //TRANSLATEUNITGOOGLE_H
