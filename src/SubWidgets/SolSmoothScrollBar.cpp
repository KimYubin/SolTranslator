// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSmoothScrollBar.h"

#include "SolSmoothComponent.h"

#include <QWheelEvent>
#include <qstyleoption.h>


namespace Sol
{
SolSmoothScrollBar::SolSmoothScrollBar(QWidget* inParent)
    : QScrollBar(inParent)
    , _smoothComponent(new SolSmoothComponent(this, [this]() { return value(); }))
    , _repeatDelay(500)
    , _repeatDuration(50)
    , _pageStepRepeatLimit(1)
{
    connect(this, &QAbstractSlider::actionTriggered, this, &SolSmoothScrollBar::onActionTriggered);

    connect(this, &QScrollBar::rangeChanged, _smoothComponent, &SolSmoothComponent::setRange);
    connect(_smoothComponent, &SolSmoothComponent::valueChanged, this, &QScrollBar::setValue);
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

    // In Qt, the angles of the lower and right wheels are negative.
    return scrollSmoothToDeltaValue(-deltaVal);
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
    // Disable the repeat action of the QScrollBar.
    // Use custom repetition.
    setRepeatAction(SliderNoAction);

    bool isSingleStep = false;
    bool isPageStep   = false;
    bool isMinMax     = false;

    switch (inAction)
    {
    case SliderSingleStepAdd: case SliderSingleStepSub: isSingleStep = true; break;
    case SliderPageStepAdd: case SliderPageStepSub: isPageStep = true; break;
    case SliderToMinimum: case SliderToMaximum: isMinMax = true; break;
    default:
        return;
    }

    // The value() has not been changed yet.
    // Since Need to move the Slider with custom implementation,
    // restore the changed sliderPosition to the value before the change.
    const int curValue  = value();
    const int targetVal = sliderPosition();
    setSliderPosition(curValue);

    if (isMinMax)
    {
        scrollSmoothToTargetValue(targetVal);
        return;
    }

    // Positive number, bottom right direction.
    const int nextDelta = targetVal - curValue;
    if (nextDelta == 0)
    {
        return;
    }

    setRepeatTimer(static_cast<SliderAction>(inAction));

    if (isSingleStep && (_isFirstAction == false))
    {
        stopRepeat();
        _smoothComponent->startContinuousSmoothAnimation(inAction == SliderSingleStepAdd);
        return;
    }

    if (isPageStep)
    {
        // Reached the press position.
        if ((nextDelta > 0 && curValue >= _pressRangeValue)
            || (nextDelta < 0 && curValue <= _pressRangeValue))
        {
            stopRepeat();
            return;
        }

        // If the next step exceeds the press position, move only up to the press position.
        if ((nextDelta > 0 && targetVal >= (_pressRangeValue - (pageStep() / 2)))
            || (nextDelta < 0 && targetVal <= (_pressRangeValue + (pageStep() / 2))))
        {
            scrollSmoothToTargetValue(_pressRangeValue);
            return;
        }

        if (_repeatStack++ >= _pageStepRepeatLimit)
        {
            _repeatStack = _pageStepRepeatLimit;
            scrollSmoothToTargetValue(_pressRangeValue);
            return;
        }
    }

    scrollSmoothToDeltaValue(nextDelta);
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

void SolSmoothScrollBar::setRepeatTimer(const SliderAction inAction)
{
    switch (inAction)
    {
    case SliderSingleStepAdd:
    case SliderSingleStepSub:
    case SliderPageStepAdd:
    case SliderPageStepSub:
        break;
    case SliderNoAction:
    case SliderToMinimum:
    case SliderToMaximum:
    case SliderMove:
    default:
        stopRepeat();
        return;
    }

    if (!_repeatTimer.isActive())
    {
        _isFirstAction = true;
        _repeatTimer.start(_repeatDelay, this);
    }

    _repeatAction = inAction;
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


    QScrollBar::mousePressEvent(inEvent);
}

void SolSmoothScrollBar::mouseReleaseEvent(QMouseEvent* inEvent)
{
    if (_repeatTimer.isActive())
    {
        stopRepeat();
    }

    _smoothComponent->stopContinuousSmoothAnimation();

    QScrollBar::mouseReleaseEvent(inEvent);
}

void SolSmoothScrollBar::timerEvent(QTimerEvent* inEvent)
{
    QScrollBar::timerEvent(inEvent);
    if (inEvent->timerId() == _repeatTimer.timerId())
    {
        if (_isFirstAction)
        {
            // was _repeatDelay time, use _repeatDuration next time.
            _repeatTimer.start(_repeatDuration, this);
            _isFirstAction = false;
        }

        triggerAction(_repeatAction);
    }
}

void SolSmoothScrollBar::stopRepeat()
{
    _repeatAction = SliderNoAction;
    _repeatStack  = 0;
    _repeatTimer.stop();
}
} // namespace Sol
