// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include "SolTranslatorCore.h"

#include <QObject>
#include <QPointer>


class SolTranslatorCore;

class AbstractManager : public QObject
{
    Q_OBJECT

public:
    explicit AbstractManager(SolTranslatorCore* parent);
    ~AbstractManager() override;

protected:
    friend class SolTranslatorCore;

    virtual void postInitialize();

    SolTranslatorCore* getSolCore() const { return _solCore; }

private:
    QPointer<SolTranslatorCore> _solCore;
};


#endif //ABSTRACTMANAGER_H
