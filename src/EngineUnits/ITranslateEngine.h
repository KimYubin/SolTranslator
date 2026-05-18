// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ITRANSLATEENGINE_H
#define SOLTRANSLATOR_ITRANSLATEENGINE_H

#include "Types/EngineId.h"
#include "Types/OptionSpec.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

#include <QObject>
#include <qicon.h>


class ITranslateEngine;
class TranslateManager;
class TranslateUnit;

using OptionMap     = std::unordered_map<OptionKey, OptionSpec, OptionKey_hasher>;
using TrUnitCreator = Callback<TranslateUnit*(TranslateManager*)>;

/** The ITranslateEngine class manages metadata for the translation engine. */
class ITranslateEngine : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(ITranslateEngine)

public:
    explicit ITranslateEngine(EngineId inEngine);
    virtual ~ITranslateEngine() override;

    virtual void postInitialize();

    const EngineId& getEngineId() const { return _engineId; }
    const QString& getDisplayName() const { return _displayName; }
    const QString& getDefaultUrl() const { return _defaultUrl; }
    const QString& getIconPath() const { return _iconPath; }
    const QIcon& getIcon() const { return _icon; }
    int getPriority() const { return _priority; }

    Expected<const OptionSpec*> getOptionSpec(const OptionKey& inKey) const;
    const OptionMap& getOptions() const { return _optionSpecs; }
    std::vector<const OptionSpec*> sortedOptionSpecList() const;

    TranslateUnit* newTrUnit(TranslateManager* inTrManager);

protected:
    void setDisplayName(const QString& inDisplayName);
    void setDefaultUrl(const QString& inDefaultUrl);
    void setIconPath(const QString& inIconPath);
    void setIcon(const QIcon& inIcon);
    void setPriority(const int inPriority);
    void setTrUnitCreator(TrUnitCreator&& inCreator);

    template <std::derived_from<TranslateUnit> T>
    void setTrUnitCreatorHelper();

    void appendOptionSpecList(std::vector<OptionSpec> inOptionSpecs);
    void setOptionSpec(OptionSpec inOptionSpec);

private:
    int optionOrder() const { return _optionOrder++; }

private:
    EngineId _engineId;
    QString _displayName;
    QString _defaultUrl;
    QString _iconPath;
    QIcon _icon;
    int _priority;
    TrUnitCreator _trUnitCreator;

    mutable int _optionOrder = 0;
    OptionMap _optionSpecs;
};

template <std::derived_from<TranslateUnit> T>
void ITranslateEngine::setTrUnitCreatorHelper()
{
    setTrUnitCreator([this](TranslateManager* inTrManager)
    {
        return new T{inTrManager, this};
    });
}


namespace TrEngineOptionKey
{
inline const OptionKey ApiKey{"ApiKey"};
}


#endif //SOLTRANSLATOR_ITRANSLATEENGINE_H
