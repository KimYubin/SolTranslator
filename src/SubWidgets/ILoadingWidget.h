// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ILOADINGWIDGET_H
#define SOLTRANSLATOR_ILOADINGWIDGET_H

#include <QWidget>

class ILoadingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ILoadingWidget(QWidget* parent = nullptr);

    virtual void run() = 0;
    virtual void stop() = 0;
};


#endif //SOLTRANSLATOR_ILOADINGWIDGET_H
