// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "LoadingBar.h"

#include <QPropertyAnimation>
#include <QSvgWidget>

LoadingBar::LoadingBar(const QString& file, QWidget* parent): QWidget(parent)
{
    svg = new QSvgWidget(file, this);

    setFixedHeight(4);
    svg->setFixedSize(350, 4);

    _animRatio     = 0;
    _prevRatio     = 0;
    _integralRatio = 0;
    _bRunning      = false;

    animation = new QPropertyAnimation(this, "animRatio", this);
    animation->setDuration(1000); // svg 좌우 길이만큼 이동하는데 걸리는 시간.
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setLoopCount(-1); // 무한 반복
}

void LoadingBar::run()
{
    _bRunning = true;
    animation->stop();
    animation->start();
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
    const int startPosX = -svg->width();
    const int svgWidth  = svg->width();
    int newPosX         = startPosX + svgWidth * _integralRatio;
    if (newPosX > width())
    {
        if (_bRunning == false)
        {
            animation->stop();
            svg->move(startPosX - 100, 0);
            svg->setVisible(false);
            return;
        }
        _integralRatio = 0;
        newPosX        = startPosX + svgWidth * _integralRatio;
    }

    svg->move(newPosX, 0);
}
