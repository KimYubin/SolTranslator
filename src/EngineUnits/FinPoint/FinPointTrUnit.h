// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINPOINT_H
#define FINPOINT_H
#include "EngineUnits/TranslateUnit.h"

class FinPointTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit FinPointTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                          , TranslateManager* parent);

protected:
    void chatTranslate(const bool bIsStreaming);

    virtual void requestTranslate() override;
    virtual void onReadyRead() override;
    virtual void replyTranslateFinished() override;

public:
    void setDebugMode(const bool inDebugMode) { bDebugMode = inDebugMode; };

private:
    bool bDebugMode = false;
};


#endif //FINPOINT_H
