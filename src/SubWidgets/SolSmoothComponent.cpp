// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSmoothComponent.h"

#include <QVariantAnimation>


SolSmoothComponent::SolSmoothComponent(QObject* inParent, Callback<int()>&& inValueCallback)
    : QObject(inParent)
    , _smoothAnim(new QVariantAnimation(this))
    , _minVal(std::numeric_limits<int>::min())
    , _maxVal(std::numeric_limits<int>::max())
{
    setCurrentValueFunctor(std::move(inValueCallback));

    connect(_smoothAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant& inVar) { emit valueChanged(inVar.toInt()); });
}

void SolSmoothComponent::setCurrentValueFunctor(Callback<int()>&& inValueCallback)
{
    _curValue = std::move(inValueCallback);
}

void SolSmoothComponent::setAnimDuration(const int inAnimDuration)
{
    _animDuration = inAnimDuration;
}

void SolSmoothComponent::setEasingCurve(const QEasingCurve& inEasingCurve)
{
    _easingCurve = inEasingCurve;
}

bool SolSmoothComponent::scrollToTargetValue(const int inTargetValue)
{
    if ( !_curValue)
    {
        return false;
    }

    _smoothAnim->stop();

    const int curVal = _curValue();

    _targetValue = std::clamp(inTargetValue, _minVal, _maxVal);
    if (curVal == _targetValue)
    {
        return false;
    }

    _smoothAnim->setDuration(_animDuration);
    _smoothAnim->setStartValue(curVal);
    _smoothAnim->setEndValue(_targetValue);
    _smoothAnim->setEasingCurve(_easingCurve);
    _smoothAnim->start();

    return true;
}

bool SolSmoothComponent::scrollToDeltaValue(const int inDeltaValue)
{
    if ( !_curValue)
    {
        return false;
    }

    const int curVal = _curValue();
    if (_smoothAnim->state() == QAbstractAnimation::Running)
    {
        _smoothAnim->stop();
    }
    else
    {
        _targetValue = curVal;
    }

    // Adding same direction.
    if ((inDeltaValue > 0 && curVal < _targetValue)
        || (inDeltaValue < 0 && curVal > _targetValue))
    {
        _targetValue = _targetValue + inDeltaValue;
    }
    else
    {
        // Opposite wheel direction.
        _targetValue = curVal + inDeltaValue;
    }

    return scrollToTargetValue(_targetValue);
}

void SolSmoothComponent::setRange(const int inMin, const int inMax)
{
    _minVal = inMin;
    _maxVal = inMax;

    if (_minVal == _maxVal)
    {
        _smoothAnim->stop();
    }
}