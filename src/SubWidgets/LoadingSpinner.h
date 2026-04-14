// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_LOADINGSPINNER_H
#define SOLTRANSLATOR_LOADINGSPINNER_H

#include "ILoadingWidget.h"

class QSvgWidget;
class QTimer;

class LoadingSpinner : public ILoadingWidget
{
    Q_OBJECT

public:
    explicit LoadingSpinner(const QString& inFile, QWidget* parent = nullptr);
    virtual void setBubbleToolTip(const QString& inToolTip) override;

    virtual void run() override;
    virtual void stop() override;

private:
    void showSvg();
    void hideSvg();
    QSvgWidget* _svg;
    QTimer* _stopTimer;
};


#endif //SOLTRANSLATOR_LOADINGSPINNER_H
