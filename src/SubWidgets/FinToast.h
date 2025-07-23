// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTOAST_H
#define FINTOAST_H

#include <QObject>


class FinToast : public QObject
{
    Q_OBJECT

    explicit FinToast(QObject* parent = nullptr);
    ~FinToast() override;

public:
    static void showToast(const QString& inMessage, const int inDuration = 1'500);
};


#endif //FINTOAST_H
