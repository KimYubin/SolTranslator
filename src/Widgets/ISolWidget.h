// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ISOLWIDGET_H
#define ISOLWIDGET_H

#include <QWidget>

namespace Sol
{
class ISolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ISolWidget(QWidget* inParent = nullptr, const Qt::WindowFlags inFlags = Qt::WindowFlags());

    ~ISolWidget() override;

    virtual void showRaiseUp();
};


} // namespace Sol

#endif //ISOLWIDGET_H
