// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSmoothScroll.h"

#include "Utils/SolLog.h"

#include <QPropertyAnimation>
#include <QWheelEvent>
#include <qstyleoption.h>

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

bool SolScrollSmoothComponent::scrollToTargetValue(const int inTargetValue
                                                 , const int inAnimDuration)
{
    _scrollAnim->stop();

    const int curVal = _scrollBar->value();
    const int minVal = _scrollBar->minimum();
    const int maxVal = _scrollBar->maximum();

    _targetValue = std::clamp(inTargetValue, minVal, maxVal);
    if (curVal == _targetValue)
    {
        return false;
    }

    _scrollAnim->setDuration(inAnimDuration);
    _scrollAnim->setStartValue(curVal);
    _scrollAnim->setEndValue(_targetValue);
    _scrollAnim->setEasingCurve(QEasingCurve::OutCubic);
    _scrollAnim->start();

    return true;
}

bool SolScrollSmoothComponent::scrollToDeltaValue(const int inDeltaValue
                                                , const int inAnimDuration)
{
    if (_scrollBar.isNull())
    {
        return false;
    }

    const int curVal = _scrollBar->value();
    if (_scrollAnim->state() == QAbstractAnimation::Running)
    {
        if (_targetValue == (curVal - inDeltaValue))
        {
            // return true;
        }
        _scrollAnim->stop();
    }
    else
    {
        _targetValue = curVal;
    }

    // Scroll direction == wheel direction
    if ((inDeltaValue < 0 && curVal < _targetValue)
        || (inDeltaValue > 0 && curVal > _targetValue))
    {
        _targetValue = _targetValue - inDeltaValue;
    }
    else
    {
        // Opposite wheel direction.
        _targetValue = curVal - inDeltaValue;
    }

    return scrollToTargetValue(_targetValue, inAnimDuration);
}


// ~===============================
// SolSmoothScrollBar
SolSmoothScrollBar::SolSmoothScrollBar(QWidget* inParent)
    : QScrollBar(inParent)
    , _smoothComponent(new SolScrollSmoothComponent(this, this))
    , _repeatDelay(500)
    , _repeatDuration(50)
    , _pageStepRepeatLimit(1)
{
    connect(this, &QAbstractSlider::actionTriggered, this, &SolSmoothScrollBar::onActionTriggered);
}

bool SolSmoothScrollBar::scrollSmoothToTargetValue(const int inTargetVal
                                                 , const int inAnimDuration)
{
    return _smoothComponent->scrollToTargetValue(inTargetVal, inAnimDuration);
}

bool SolSmoothScrollBar::scrollSmoothToDeltaValue(const int inDeltaVal
                                                , const int inAnimDuration)
{
    return _smoothComponent->scrollToDeltaValue(inDeltaVal, inAnimDuration);
}

bool SolSmoothScrollBar::scrollSmoothToDeltaAngle(const float inDeltaAngle)
{
    const float deltaWheelStep = inDeltaAngle / 120.f;
    const int deltaVal = std::round(singleStep() * deltaWheelStep);

    return scrollSmoothToDeltaValue(deltaVal);
}

bool SolSmoothScrollBar::isHorizontal() const
{
    return (orientation() == Qt::Horizontal);
}

void SolSmoothScrollBar::setRepeatDelay(const int inRepeatDelay)
{
    _repeatDelay = inRepeatDelay;
}

void SolSmoothScrollBar::setRepeatDuration(const int inRepeatDuration)
{
    _repeatDuration = inRepeatDuration;
}

void SolSmoothScrollBar::setPageStepRepeatLimit(const int inPageStepRepeatLimit)
{
    _pageStepRepeatLimit = inPageStepRepeatLimit;
}


void SolSmoothScrollBar::onActionTriggered(const int inAction)
{
    const bool isSingleStep = (inAction == SliderSingleStepAdd || inAction == SliderSingleStepSub);
    const bool isPageStep = (inAction == SliderPageStepAdd || inAction == SliderPageStepSub);

    if (isSingleStep)
    {
        setRepeatDuration(25);
    }
    else if (isPageStep)
    {
        setRepeatDuration(50);
    }

    switch (inAction)
    {
    case SliderSingleStepAdd:
    case SliderSingleStepSub:
    case SliderPageStepAdd:
    case SliderPageStepSub:
    case SliderToMinimum:
    case SliderToMaximum:
    {
        setRepeatAction(SliderNoAction);
        _repeatAction = static_cast<SliderAction>(inAction);

        // actionTriggered() 신호 발생 이전과 이후에,
        // 각각 sliderPosition과 value가 수정됩니다.
        // 자체 타이머를 통해 Slider를 움직여야하므로, 
        // 변경된 sliderPosition을 변경 전인 value값으로 복구합니다.

        _prvValue  = value();
        _targetPos = sliderPosition();

        setSliderPosition(_prvValue);

        if (isPageStep)
        {
            if ((_prvValue < _targetPos && _pressRangeValue <= _targetPos)
                || (_prvValue > _targetPos && _pressRangeValue >= _targetPos))
            {
                break;
            }

            if (_repeatStack++ >= _pageStepRepeatLimit)
            {
                _repeatStack = _pageStepRepeatLimit;
                _targetPos   = _pressRangeValue;
                scrollSmoothToTargetValue(_targetPos);
                break;
            }
        }
        scrollSmoothToDeltaValue(_prvValue - _targetPos);
        break;
    }
    default:;
    }
}

void SolSmoothScrollBar::wheelEvent(QWheelEvent* inEvent)
{
    QPoint angleDelta = inEvent->angleDelta();

    if (inEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        angleDelta.setX(inEvent->angleDelta().y());
        angleDelta.setY(inEvent->angleDelta().x());
    }

    inEvent->ignore();
    const bool isHorizontalWheel = qAbs(angleDelta.x()) > qAbs(angleDelta.y());

    if (!isHorizontalWheel && angleDelta.x() != 0 && isHorizontal())
    {
        return;
    }

    // The Qt default uses the inverted value of angleDelta.x().
    // Here, Use the input value as it is without inverting the left-right scroll.
    const int delta = isHorizontalWheel ? (angleDelta.x()) : angleDelta.y();

    if (scrollSmoothToDeltaAngle(delta))
    {
        inEvent->accept();
    }
}

int SolSmoothScrollBar::pixelPosToRangeValue(const int inPos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    const QRect gr = style()->subControlRect(QStyle::CC_ScrollBar
                                           , &opt
                                           , QStyle::SC_ScrollBarGroove
                                           , this);
    const QRect sr = style()->subControlRect(QStyle::CC_ScrollBar
                                           , &opt
                                           , QStyle::SC_ScrollBarSlider
                                           , this);

    int sliderMin, sliderMax, sliderLength;

    if (isHorizontal())
    {
        sliderLength = sr.width();
        sliderMin    = gr.x();
        sliderMax    = gr.right() - sliderLength + 1;
        if (layoutDirection() == Qt::RightToLeft)
        {
            opt.upsideDown = !opt.upsideDown;
        }
    }
    else
    {
        sliderLength = sr.height();
        sliderMin    = gr.y();
        sliderMax    = gr.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition(minimum()
                                         , maximum()
                                         , inPos - sliderMin
                                         , sliderMax - sliderMin
                                         , opt.upsideDown);
}

void SolSmoothScrollBar::mousePressEvent(QMouseEvent* inEvent)
{
    if (!_repeatActionTimer.isActive())
    {
        _isAfterThreshold = true;
        _repeatActionTimer.start(_repeatDelay, this);
    }

    QStyleOptionSlider opt;
    initStyleOption(&opt);
    opt.keyboardModifiers = inEvent->modifiers();

    const QRect sr = style()->subControlRect(QStyle::CC_ScrollBar
                                           , &opt
                                           , QStyle::SC_ScrollBarSlider
                                           , this);
    const QPoint clickPos = inEvent->position().toPoint();
    const QPoint pixelPos = clickPos - sr.center() + sr.topLeft();

    _pressRangeValue = pixelPosToRangeValue(isHorizontal() ? pixelPos.x() : pixelPos.y());

    QScrollBar::mousePressEvent(inEvent);
}

void SolSmoothScrollBar::mouseReleaseEvent(QMouseEvent* inEvent)
{
    if (_repeatActionTimer.isActive())
    {
        _repeatAction = SliderNoAction;
        _repeatStack  = 0;
        _repeatActionTimer.stop();
    }

    QScrollBar::mouseReleaseEvent(inEvent);
}

void SolSmoothScrollBar::timerEvent(QTimerEvent* inEvent)
{
    QScrollBar::timerEvent(inEvent);
    if (inEvent->timerId() == _repeatActionTimer.timerId())
    {
        if (_isAfterThreshold)
        {
            // was threshold time, use repeat time next time
            _repeatActionTimer.start(_repeatDuration, this);
            _isAfterThreshold = false;
        }

        triggerAction(_repeatAction);
    }
}
