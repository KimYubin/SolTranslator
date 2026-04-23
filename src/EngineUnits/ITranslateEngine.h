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
using TrEngineMap   = std::unordered_map<EngineId, QPointer<ITranslateEngine>, EngineId_hasher>;

class ITranslateEngine : public QObject
{
    Q_OBJECT

public:
    explicit ITranslateEngine(const EngineId& inEngine);
    virtual ~ITranslateEngine() override;

    static const TrEngineMap& allTrUnitCreators();
    static std::vector<ITranslateEngine*> sortedTranslateEngineList();

    static std::expected<TranslateUnit*, QString> newTrUnit(const EngineId& inEngine, TranslateManager* inTrManager);

    EngineId getEngineId() const { return _engineId; }
    QString getDisplayName() const { return _displayName; }
    QString getIconPath() const { return _iconPath; }
    int getPriority() const { return _priority; }

protected:
    void setDisplayName(const QString& inDisplayName);
    void setIconPath(const QString& inIconPath);
    void setPriority(const int inPriority);
    void setTrUnitCreator(TrUnitCreator&& inCreator);

private:
    EngineId _engineId;
    QString _displayName;
    QString _iconPath;
    int _priority;
    TrUnitCreator _trUnitCreator;


    Q_DISABLE_COPY_MOVE(ITranslateEngine)
};

#endif //SOLTRANSLATOR_ITRANSLATEENGINE_H
