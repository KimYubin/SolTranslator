// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H
#include <QObject>


class FinTranslatorCore;

class AbstractManager : public QObject
{
    Q_OBJECT

public:
    explicit AbstractManager(FinTranslatorCore* parent);

};


#endif //ABSTRACTMANAGER_H
