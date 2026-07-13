// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSmoothScroll.h"

#include <QPropertyAnimation>
#include <QWheelEvent>

// ~===============================
// SolScrollSmoothComponent
SolScrollSmoothComponent::SolScrollSmoothComponent(QWidget* inParent, QScrollBar* inScrollBar)
    : QObject(inParent)
    , _scrollAnim(new QPropertyAnimation(inScrollBar, "value", this))
{
    setScrollBar(inScrollBar);
}

void SolScrollSmoothComponent::setScrollBar(QScrollBar* inScrollBar)
{
    if (_scrollBar)
    {
        disconnect(_scrollBar, &QScrollBar::rangeChanged, _scrollAnim, nullptr);
    }

    _scrollAnim->setTargetObject(inScrollBar);

    _scrollBar = inScrollBar;
    if (_scrollBar.isNull())
    {
        return;
    }

    connect(_scrollBar, &QScrollBar::rangeChanged, _scrollAnim, [this](int min, int max)
    {
        if (min == max)
        {
            _scrollAnim->stop();
        }
    });
}

bool SolScrollSmoothComponent::smoothWheel(const float inAngleDelta)
{
    if (_scrollBar.isNull())
    {
        return false;
    }

    const int curVal = _scrollBar->value();
    if (_scrollAnim->state() != QAbstractAnimation::Running)
    {
        _targetValue = curVal;
    }
    else
    {
        _scrollAnim->stop();
    }

    const float deltaWheelStep = inAngleDelta / 120.f;
    const int deltaVal = _scrollBar->singleStep() * deltaWheelStep;

    // Scroll direction == wheel direction
    if ((deltaWheelStep < 0 && curVal < _targetValue) || (deltaWheelStep > 0 && curVal > _targetValue))
    {
        _targetValue = _targetValue - deltaVal;
    }
    else
    {
        // Opposite wheel direction.
        _targetValue = curVal - deltaVal;
    }

    const int minVal = _scrollBar->minimum();
    const int maxVal = _scrollBar->maximum();
    _targetValue     = std::clamp(_targetValue, minVal, maxVal);
    if (curVal == _targetValue)
    {
        return false;
    }

    _scrollAnim->setDuration(200);
    _scrollAnim->setStartValue(curVal);
    _scrollAnim->setEndValue(_targetValue);
    _scrollAnim->setEasingCurve(QEasingCurve::OutCubic);
    _scrollAnim->start();

    return true;
}


// ~===============================
// SolSmoothScrollBar
SolSmoothScrollBar::SolSmoothScrollBar(QWidget* inParent)
    : QScrollBar(inParent)
    , _smoothComponent(new SolScrollSmoothComponent(this, this))
{}

bool SolSmoothScrollBar::setSmoothValue(const float inAngleDelta)
{
    return _smoothComponent->smoothWheel(inAngleDelta);
}

void SolSmoothScrollBar::wheelEvent(QWheelEvent* event)
{
    const QPoint angleDelta = event->angleDelta();

    event->ignore();
    const bool isHorizontal = qAbs(angleDelta.x()) > qAbs(angleDelta.y());

    if (!isHorizontal && angleDelta.x() != 0 && orientation() == Qt::Horizontal)
    {
        return;
    }

    const int delta = isHorizontal ? (-angleDelta.x()) : angleDelta.y();

    if (_smoothComponent->smoothWheel(delta))
    {
        event->accept();
    }
}
