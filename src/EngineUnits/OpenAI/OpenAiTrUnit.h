// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "EngineUnits/ITranslateEngine.h"
#include "EngineUnits/TranslateUnit.h"


class OpenAiTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit OpenAiTrUnit(TranslateManager* parent, ITranslateEngine* inEngine);

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

class OpenAiEngine : public ITranslateEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OpenAiEngine)

public:
    explicit OpenAiEngine();
    ~OpenAiEngine() override;
};

#endif //TRANSLATEUNITOPENAI_H
