// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITGOOGLE_H
#define TRANSLATEUNITGOOGLE_H

#include "EngineUnits/ITranslateEngine.h"
#include "EngineUnits/TranslateUnit.h"


class GoogleTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit GoogleTrUnit(TranslateManager* parent, ITranslateEngine* inEngine);

    virtual void requestTranslate() override;

protected:
    virtual void onReadyRead() override;
    virtual QString replyTranslateFinished() override;
};


namespace EngineIds
{
inline const EngineId Google{"Google"};
inline const EngineId& defaultEngine = Google;
}
class GoogleEngine : public ITranslateEngine
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(GoogleEngine)

public:
    explicit GoogleEngine();
    ~GoogleEngine() override;

};

#endif //TRANSLATEUNITGOOGLE_H
