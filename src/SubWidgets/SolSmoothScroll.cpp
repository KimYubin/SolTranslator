// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSmoothScroll.h"

#include "Utils/SolLog.h"

#include <QPropertyAnimation>
#include <QWheelEvent>
#include <qstyleoption.h>

// ~===============================
// SolSmoothScrollComponent
SolSmoothScrollComponent::SolSmoothScrollComponent(QWidget* inParent, QScrollBar* inScrollBar)
    : QObject(inParent)
    , _scrollAnim(new QPropertyAnimation(inScrollBar, "value", this))
{
    setScrollBar(inScrollBar);
}

void SolSmoothScrollComponent::setScrollBar(QScrollBar* inScrollBar)
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

void SolSmoothScrollComponent::setAnimDuration(const int inAnimDuration)
{
    _animDuration = inAnimDuration;
}

void SolSmoothScrollComponent::setEasingCurve(const QEasingCurve& inEasingCurve)
{
    _easingCurve = inEasingCurve;
}

bool SolSmoothScrollComponent::scrollToTargetValue(const int inTargetValue)
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

    _scrollAnim->setDuration(_animDuration);
    _scrollAnim->setStartValue(curVal);
    _scrollAnim->setEndValue(_targetValue);
    _scrollAnim->setEasingCurve(_easingCurve);
    _scrollAnim->start();

    return true;
}

bool SolSmoothScrollComponent::scrollToDeltaValue(const int inDeltaValue)
{
    if (_scrollBar.isNull())
    {
        return false;
    }

    const int curVal = _scrollBar->value();
    if (_scrollAnim->state() == QAbstractAnimation::Running)
    {
        _scrollAnim->stop();
    }
    else
    {
        _targetValue = curVal;
    }

    // Adding same direction.
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

    return scrollToTargetValue(_targetValue);
}


// ~===============================
// SolSmoothScrollBar
SolSmoothScrollBar::SolSmoothScrollBar(QWidget* inParent)
    : QScrollBar(inParent)
    , _smoothComponent(new SolSmoothScrollComponent(this, this))
    , _repeatDelay(500)
    , _repeatDuration(50)
    , _pageStepRepeatLimit(1)
{
    connect(this, &QAbstractSlider::actionTriggered, this, &SolSmoothScrollBar::onActionTriggered);
}

bool SolSmoothScrollBar::scrollSmoothToTargetValue(const int inTargetVal)
{
    stopRepeat();

    return _smoothComponent->scrollToTargetValue(inTargetVal);
}

bool SolSmoothScrollBar::scrollSmoothToDeltaValue(const int inDeltaVal)
{
    return _smoothComponent->scrollToDeltaValue(inDeltaVal);
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

        // 음수면 아래/오른쪽 방향
        const int nextDelta = _prvValue - _targetPos;

        if (nextDelta == 0)
        {
            break;
        }

        if (isPageStep)
        {
            // 클릭 위치에 도달
            if ((nextDelta < 0 && _prvValue >= _pressRangeValue)
                || (nextDelta > 0 && _prvValue <= _pressRangeValue))
            {
                break;
            }

            // 다음 스탭이 press 위치를 넘어갈 경우. press 위치까지만 이동.
            if ((nextDelta < 0 && _targetPos >= (_pressRangeValue - (pageStep() / 2)))
                ||(nextDelta > 0 && _targetPos <= (_pressRangeValue + (pageStep() / 2))))
            {
                scrollSmoothToTargetValue(_pressRangeValue);
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

        scrollSmoothToDeltaValue(nextDelta);
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

    if (!isHorizontalWheel && (angleDelta.x() != 0) && isHorizontal())
    {
        return;
    }

    // The Qt default uses the inverted value of angleDelta.x().
    // Here, Use the input value as it is without inverting the left-right scroll.
    const int delta = isHorizontalWheel ? angleDelta.x() : angleDelta.y();

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

    if (!_repeatActionTimer.isActive())
    {
        _isFirstAction = true;
        _repeatActionTimer.start(_repeatDelay, this);
    }

    QScrollBar::mousePressEvent(inEvent);
}

void SolSmoothScrollBar::mouseReleaseEvent(QMouseEvent* inEvent)
{
    if (_repeatActionTimer.isActive())
    {
        stopRepeat();
    }

    QScrollBar::mouseReleaseEvent(inEvent);
}

void SolSmoothScrollBar::timerEvent(QTimerEvent* inEvent)
{
    QScrollBar::timerEvent(inEvent);
    if (inEvent->timerId() == _repeatActionTimer.timerId())
    {
        if (_isFirstAction)
        {
            // was threshold time, use repeat time next time
            _repeatActionTimer.start(_repeatDuration, this);
            _isFirstAction = false;
        }

        triggerAction(_repeatAction);
    }
}

void SolSmoothScrollBar::stopRepeat()
{
    _repeatAction = SliderNoAction;
    _repeatStack  = 0;
    _repeatActionTimer.stop();
}
