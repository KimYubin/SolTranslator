// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLPOINT_H
#define SOLPOINT_H

#include "EngineUnits/ITranslateEngine.h"
#include "EngineUnits/TranslateUnit.h"


namespace Sol
{
class FinPointTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit FinPointTrUnit(TranslateManager* inParent, ITranslateEngine* inEngine);

protected:
    void requestTranslateImpl() override;

    void chatTranslate(const bool inIsStreaming);

    void onReadyRead() override;
    QString replyTranslateFinished() override;

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

} // namespace Sol

#endif //SOLPOINT_H
