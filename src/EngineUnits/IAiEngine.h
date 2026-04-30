// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_IAIENGINE_H
#define SOLTRANSLATOR_IAIENGINE_H
#include "ITranslateEngine.h"


class AiTranslateUnit;

class IAiEngine : public ITranslateEngine
{
    Q_OBJECT

public:
    explicit IAiEngine(const EngineId& inEngine);
    virtual ~IAiEngine() override;

    const QString& getDefaultModel() const { return _defaultModel; }
    const QString& getDefaultPrompt() const { return _defaultPrompt; }
    double getDefaultTemperature() const { return _defaultTemperature; }

protected:
    void setDefaultModel(const QString& inDefaultModel);
    void setDefaultPrompt(const QString& inDefaultPrompt);
    void setDefaultTemperature(const double inDefaultTemperature);

    template <std::derived_from<AiTranslateUnit> T>
    void setTrUnitCreatorHelper()
    {
        setTrUnitCreator([this](TranslateManager* inTrManager)
        {
            return new T{inTrManager, this};
        });
    }

private:
    QString _defaultModel;
    QString _defaultPrompt;
    double _defaultTemperature;
};


#endif //SOLTRANSLATOR_IAIENGINE_H
