// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef LOADINGBAR_H
#define LOADINGBAR_H

#include "ILoadingWidget.h"

class QPropertyAnimation;
class QSvgWidget;

namespace Sol
{
class LoadingBar : public ILoadingWidget
{
    Q_OBJECT

    Q_PROPERTY(float animRatio READ getAnimRatio WRITE setAnimRatio)

public:
    explicit LoadingBar(const QString& inFile, QWidget* inParent = nullptr);

    void run() override;
    void stop() override;

private:
    QSvgWidget* _svg;
    QPropertyAnimation* _animation;

    float getAnimRatio() const { return _animRatio; }
    void setAnimRatio(const float inAnimRatio);
    float _animRatio;
    float _prevRatio;
    float _integralRatio;

    bool _bRunning;
};


} // namespace Sol

#endif //LOADINGBAR_H
