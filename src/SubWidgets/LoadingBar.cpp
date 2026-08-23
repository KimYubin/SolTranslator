// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "LoadingBar.h"

#include <QPropertyAnimation>
#include <QSvgWidget>

namespace Sol
{
LoadingBar::LoadingBar(const QString& inFile, QWidget* inParent)
    : ILoadingWidget(inParent)
{
    _svg = new QSvgWidget(inFile, this);

    setFixedHeight(4);
    _svg->setFixedSize(350, 4);

    _animRatio     = 0;
    _prevRatio     = 0;
    _integralRatio = 0;
    _bRunning      = false;

    _animation = new QPropertyAnimation(this, "animRatio", this);
    _animation->setDuration(1000); // _svg 좌우 길이만큼 이동하는데 걸리는 시간.
    _animation->setEasingCurve(QEasingCurve::Linear);
    _animation->setStartValue(0);
    _animation->setEndValue(1);
    _animation->setLoopCount(-1); // 무한 반복
}

void LoadingBar::run()
{
    _bRunning = true;
    _animation->stop();
    _animation->start();
}

void LoadingBar::stop()
{
    _bRunning = false;
}

void LoadingBar::setAnimRatio(const float inAnimRatio)
{
    float deltaRatio = inAnimRatio - _prevRatio;

    // 처음으로 돌아가는 경우 delta 계산.
    deltaRatio = (deltaRatio > 0) ? (deltaRatio) : (1 + deltaRatio);

    _prevRatio = inAnimRatio;
    _integralRatio += deltaRatio;
    const int startPosX = -_svg->width();
    const int svgWidth  = _svg->width();
    int newPosX         = startPosX + svgWidth * _integralRatio;
    if (newPosX > width())
    {
        if (_bRunning == false)
        {
            _animation->stop();
            _svg->move(startPosX - 100, 0);
            _svg->setVisible(false);
            return;
        }
        _integralRatio = 0;
        newPosX        = startPosX + svgWidth * _integralRatio;
    }

    _svg->move(newPosX, 0);
}
} // namespace Sol
