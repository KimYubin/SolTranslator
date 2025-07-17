// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef PALETTEMANGER_H
#define PALETTEMANGER_H
#include <QObject>


class StyleManger  : public QObject
{
    Q_OBJECT

public:
    explicit StyleManger(QObject* parent = nullptr);
    ~StyleManger() override;

    static void applyTheme(const QString& inThemeName = "dark");
};



#endif //PALETTEMANGER_H
