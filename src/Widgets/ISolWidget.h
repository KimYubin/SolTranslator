// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ISOLWIDGET_H
#define ISOLWIDGET_H

#include <QWidget>

class ISolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ISolWidget(QWidget* parent = nullptr, const Qt::WindowFlags flags = Qt::WindowFlags());

    ~ISolWidget() override;

};


#endif //ISOLWIDGET_H
