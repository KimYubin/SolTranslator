// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ITRANSLATEENGINE_H
#define SOLTRANSLATOR_ITRANSLATEENGINE_H

#include "Types/EngineId.h"
#include "Types/OptionData.h"
#include "Types/SolExpected.h"

#include <QObject>

#include <functional>


class ITranslateEngine;
class TranslateManager;
class TranslateUnit;

using OptionMap     = std::unordered_map<OptionKey, OptionData, OptionKey_hasher>;
using TrUnitCreator = std::move_only_function<TranslateUnit*(TranslateManager*)>;

/** The ITranslateEngine class manages metadata for the translation engine. */
class ITranslateEngine : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(ITranslateEngine)

public:
    explicit ITranslateEngine(const EngineId& inEngine);
    virtual ~ITranslateEngine() override;

    const EngineId& getEngineId() const { return _engineId; }
    const QString& getDisplayName() const { return _displayName; }
    const QString& getDefaultUrl() const { return _defaultUrl; }
    const QString& getIconPath() const { return _iconPath; }
    int getPriority() const { return _priority; }

    Expected<const OptionData*> getOptionData(const OptionKey& inKey) const;
    std::vector<const OptionData*> sortedOptionDataList() const;

    TranslateUnit* newTrUnit(TranslateManager* inTrManager);

protected:
    void setDisplayName(const QString& inDisplayName);
    void setDefaultUrl(const QString& inDefaultUrl);
    void setIconPath(const QString& inIconPath);
    void setPriority(const int inPriority);
    void setTrUnitCreator(TrUnitCreator&& inCreator);

    template <std::derived_from<TranslateUnit> T>
    void setTrUnitCreatorHelper();

    void appendOptionDataList(const std::vector<OptionData>& inOptionDatas);
    void setOptionData(const OptionData& inOptionData);

private:
    EngineId _engineId;
    QString _displayName;
    QString _defaultUrl;
    QString _iconPath;
    int _priority;
    TrUnitCreator _trUnitCreator;

    OptionMap _optionDatas;
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
