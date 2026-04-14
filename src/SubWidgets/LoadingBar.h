// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef LOADINGBAR_H
#define LOADINGBAR_H

#include "ILoadingWidget.h"


class QPropertyAnimation;
class QSvgWidget;

class LoadingBar : public ILoadingWidget
{
    Q_OBJECT

    Q_PROPERTY(float animRatio READ getAnimRatio WRITE setAnimRatio)

public:
    explicit LoadingBar(const QString& inFile, QWidget* parent = nullptr);

    virtual void run() override;
    virtual void stop() override;

private:
    QSvgWidget* svg;
    QPropertyAnimation* animation;

    float getAnimRatio() const { return _animRatio; }
    void setAnimRatio(const float inAnimRatio);
    float _animRatio;
    float _prevRatio;
    float _integralRatio;

    bool _bRunning;
};


#endif //LOADINGBAR_H
