// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "EngineUnits/IAiEngine.h"
#include "EngineUnits/TranslateUnit.h"


class OpenAiTrUnit : public TranslateUnit
{
    Q_OBJECT

public:
    explicit OpenAiTrUnit(TranslateManager* inParent, ITranslateEngine* inEngine);

protected:
    void requestTranslateImpl() override;

    void chatTranslate(const bool inIsStreaming);

    void onReadyRead() override;
    QString replyTranslateFinished() override;

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
