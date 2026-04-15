// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLPOINT_H
#define SOLPOINT_H

#include "EngineUnits/TranslateUnit.h"


class FinPointTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit FinPointTrUnit(TranslateManager* parent);

    virtual void requestTranslate() override;

protected:
    void chatTranslate(const bool inIsStreaming);

    virtual void onReadyRead() override;
    virtual QString replyTranslateFinished() override;

public:
    void setDebugMode(const bool inIsDebugMode) { _isDebugMode = inIsDebugMode; }

private:
    bool _isDebugMode = false;
};


#endif //SOLPOINT_H
