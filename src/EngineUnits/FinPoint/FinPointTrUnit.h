// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLPOINT_H
#define SOLPOINT_H

#include "EngineUnits/ITranslateEngine.h"
#include "EngineUnits/TranslateUnit.h"


class FinPointTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit FinPointTrUnit(TranslateManager* parent, ITranslateEngine* inEngine);

    virtual void requestTranslate() override;

protected:
    void chatTranslate(const bool inIsStreaming);

    virtual void onReadyRead() override;
    virtual QString replyTranslateFinished() override;

    QString chunkToContent();
};


namespace EngineIds
{
inline const EngineId FinPoint{"FinPoint"};
}

class FinPointEngine : public ITranslateEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(FinPointEngine)

public:
    explicit FinPointEngine();
    ~FinPointEngine() override;
};

#endif //SOLPOINT_H
