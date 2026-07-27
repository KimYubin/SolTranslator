// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHCOMPONENT_H
#define SOLTRANSLATOR_SOLSMOOTHCOMPONENT_H

#include "Types/SolTypes.h"

#include <QEasingCurve>
#include <QObject>


class QVariantAnimation;

/**
 * The SolSmoothComponent class is a component class
 * that provides a smooth wheel scrolling feature.
 */
class SolSmoothComponent : public QObject
{
    Q_OBJECT

public:
    explicit SolSmoothComponent(QObject* inParent, Callback<int()>&& inValueCallback);

    void setCurrentValueFunctor(Callback<int()>&& inValueCallback);

    void setAnimDuration(const int inAnimDuration);
    void setEasingCurve(const QEasingCurve& inEasingCurve);

    bool scrollToTargetValue(const int inTargetValue);
    bool scrollToDeltaValue(const int inDeltaValue);

    void setRange(const int inMin, const int inMax);

    void startContinuousSmoothAnimation(const bool inIsToAdd);
    void stopContinuousSmoothAnimation();

signals:
    void valueChanged(int inValue);

protected:
    QVariantAnimation* _smoothAnim;

    Callback<int()> _curValue;

    int _minVal;
    int _maxVal;

    QEasingCurve _easingCurve = QEasingCurve::OutCubic;
    int _animDuration = 200;

    int _targetValue = 0;
};


#endif //SOLTRANSLATOR_SOLSMOOTHCOMPONENT_H
