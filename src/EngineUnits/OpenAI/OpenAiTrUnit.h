// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "EngineUnits/AiTranslateUnit.h"
#include "EngineUnits/IAiEngine.h"


class OpenAiTrUnit final : public AiTranslateUnit
{
    Q_OBJECT

public:
    explicit OpenAiTrUnit(TranslateManager* parent, IAiEngine* inEngine);

    virtual void requestTranslate() override;

protected:
    void chatTranslate(const bool inIsStreaming);

    virtual void onReadyRead() override;
    virtual QString replyTranslateFinished() override;

    QString chunkToContent();
};


namespace EngineIds
{
inline const EngineId OpenAI{"OpenAI"};
}

class OpenAiEngine : public IAiEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OpenAiEngine)

public:
    explicit OpenAiEngine();
    ~OpenAiEngine() override;
};

#endif //TRANSLATEUNITOPENAI_H
