// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H
#include <QObject>


class SolTranslatorCore;

class AbstractManager : public QObject
{
    Q_OBJECT

public:
    explicit AbstractManager(SolTranslatorCore* parent);

};


#endif //ABSTRACTMANAGER_H
