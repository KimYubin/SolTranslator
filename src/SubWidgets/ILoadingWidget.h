// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ILOADINGWIDGET_H
#define SOLTRANSLATOR_ILOADINGWIDGET_H

#include <QWidget>

namespace Sol
{
class ILoadingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ILoadingWidget(QWidget* inParent = nullptr);

    virtual void run() = 0;
    virtual void stop() = 0;
};


} // namespace Sol

#endif //SOLTRANSLATOR_ILOADINGWIDGET_H
