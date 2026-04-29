// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ITRANSLATEENGINE_H
#define SOLTRANSLATOR_ITRANSLATEENGINE_H
#include "Types/EngineId.h"

#include <QObject>

#include <expected>
#include <functional>


class ITranslateEngine;
class TranslateManager;
class TranslateUnit;

using TrUnitCreator = std::move_only_function<TranslateUnit*(TranslateManager*)>;

/** The ITranslateEngine class registers and manages metadata for the translation engine. */
class ITranslateEngine : public QObject
{
    Q_OBJECT

public:
    explicit ITranslateEngine(const EngineId& inEngine);
    virtual ~ITranslateEngine() override;

    static std::vector<QPointer<ITranslateEngine>> sortedTranslateEngineList();

    static std::expected<TranslateUnit*, QString> newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager);

    const EngineId& getEngineId() const { return _engineId; }
    const QString& getDisplayName() const { return _displayName; }
    const QString& getDefaultUrl() const { return _defaultUrl; }
    const QString& getDefaultPrompt() const { return _defaultPrompt; }
    const QString& getIconPath() const { return _iconPath; }
    int getPriority() const { return _priority; }

protected:
    void setDisplayName(const QString& inDisplayName);
    void setDefaultUrl(const QString& inDefaultUrl);
    void setDefaultPrompt(const QString& inDefaultPrompt);
    void setIconPath(const QString& inIconPath);
    void setPriority(const int inPriority);
    void setTrUnitCreator(TrUnitCreator&& inCreator);

    template <std::derived_from<TranslateUnit> T>
    void setTrUnitCreatorHelper();

private:
    EngineId _engineId;
    QString _displayName;
    QString _defaultUrl;
    QString _defaultPrompt;
    QString _iconPath;
    int _priority;
    TrUnitCreator _trUnitCreator;


    Q_DISABLE_COPY_MOVE(ITranslateEngine)
};

template <std::derived_from<TranslateUnit> T>
void ITranslateEngine::setTrUnitCreatorHelper()
{
    setTrUnitCreator([this](TranslateManager* inTrManager)
    {
        return new T{inTrManager, this};
    });
}

#endif //SOLTRANSLATOR_ITRANSLATEENGINE_H
