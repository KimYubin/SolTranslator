// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "EngineUnits/TranslateUnit.h"


class OpenAiTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit OpenAiTrUnit(TranslateManager* parent);

protected:
    void chatTranslate(const bool inIsStreaming);

    virtual void requestTranslate() override;
    virtual void onReadyRead() override;
    virtual void replyTranslateFinished() override;
};


#endif //TRANSLATEUNITOPENAI_H
