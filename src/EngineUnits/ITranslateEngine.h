// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ITRANSLATEENGINE_H
#define SOLTRANSLATOR_ITRANSLATEENGINE_H
#include "Types/SolTypes.h"

#include <functional>


class ITranslateEngine;
class TranslateManager;
class TranslateUnit;

using TrUnitCreator = std::move_only_function<TranslateUnit*(TranslateManager*)>;
using TrEngineMap   = std::unordered_map<EngineType, QPointer<ITranslateEngine>>;

class ITranslateEngine : public QObject
{
    Q_OBJECT

public:
    explicit ITranslateEngine(const EngineType inEngine);
    virtual ~ITranslateEngine() override;

    static const TrEngineMap& allTrUnitCreators();
    static TranslateUnit* newTrUnit(const EngineType inEngine, TranslateManager* inTrManager);

    QString getDisplayName() const { return _displayName; }
    QString getIconPath() const { return _iconPath; }

protected:
    void setDisplayName(const QString& inDisplayName);
    void setIconPath(const QString& inIconPath);
    void setTrUnitCreator(TrUnitCreator&& inCreator);

private:
    EngineType _engineType;
    QString _displayName;
    QString _iconPath;
    TrUnitCreator _trUnitCreator;


    Q_DISABLE_COPY_MOVE(ITranslateEngine)
};

#endif //SOLTRANSLATOR_ITRANSLATEENGINE_H
