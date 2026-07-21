// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHSCROLL_H
#define SOLTRANSLATOR_SOLSMOOTHSCROLL_H

#include <QAbstractScrollArea>
#include <QEasingCurve>
#include <QPointer>
#include <QScrollBar>
#include <QWheelEvent>


class QPropertyAnimation;

/**
 * The SolSmoothScrollComponent class is a component class
 * that provides a smooth wheel scrolling feature.
 */
class SolSmoothScrollComponent : public QObject
{
public:
    explicit SolSmoothScrollComponent(QWidget* inParent, QScrollBar* inScrollBar);
    void setScrollBar(QScrollBar* inScrollBar);

    void setAnimDuration(const int inAnimDuration);
    void setEasingCurve(const QEasingCurve& inEasingCurve);

    bool scrollToTargetValue(const int inTargetValue);
    bool scrollToDeltaValue(const int inDeltaValue);

protected:
    QPointer<QScrollBar> _scrollBar;
    QPropertyAnimation* _scrollAnim;

    QEasingCurve _easingCurve = QEasingCurve::OutCubic;
    int _animDuration = 200;

    int _targetValue = 0;
};

/**
 * The SolSmoothScrollBar class is a QScrollBar-derived class
 * that provides smooth wheel scrolling.
 */
class SolSmoothScrollBar : public QScrollBar
{
public:
    explicit SolSmoothScrollBar(QWidget* inParent = nullptr);

    bool scrollSmoothToTargetValue(const int inTargetVal);
    bool scrollSmoothToDeltaValue(const int inDeltaVal);

    bool scrollSmoothToDeltaAngle(const float inDeltaAngle);
    bool isHorizontal() const;

    void setRepeatDelay(const int inRepeatDelay);
    void setRepeatDuration(const int inRepeatDuration);
    void setPageStepRepeatLimit(const int inPageStepRepeatLimit);

    void onActionTriggered(const int inAction);

    int pixelPosToRangeValue(const int inPos) const;

protected:
    virtual void wheelEvent(QWheelEvent* inEvent) override;
    virtual void mousePressEvent(QMouseEvent* inEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent* inEvent) override;
    virtual void timerEvent(QTimerEvent* inEvent) override;

private:
    void stopRepeat();

protected:
    SolSmoothScrollComponent* _smoothComponent;

    SliderAction _repeatAction = SliderNoAction;

    QBasicTimer _repeatTimer;

    int _repeatDelay;
    int _repeatDuration;
    int _pageStepRepeatLimit; // Move directly to the target point after the Repeat limit.

    int _repeatStack       = 0;
    int _pressRangeValue   = 0;
    bool _isFirstAction = false;
};

/**
 * The SolSmoothAbstractScrollArea class is a ScrollArea class
 * that provides smooth wheel scrolling.
 *
 * @tparam BaseType requires QAbstractScrollArea-derived.
 */
template <std::derived_from<QAbstractScrollArea> BaseType>
class SolSmoothAbstractScrollArea : public BaseType
{
public:
    using Base = BaseType;

    explicit SolSmoothAbstractScrollArea(QWidget* inParent = nullptr)
        : Base(inParent)
    {
        Base::setHorizontalScrollBar(new SolSmoothScrollBar(this));
        Base::setVerticalScrollBar(new SolSmoothScrollBar(this));
    };

protected:
    virtual void wheelEvent(QWheelEvent* inEvent) override
    {
        const QPoint angleDelta = inEvent->angleDelta();
        bool isHorizontal = qAbs(angleDelta.x()) > qAbs(angleDelta.y());

        if (inEvent->modifiers().testFlag(Qt::ShiftModifier))
        {
            isHorizontal = !isHorizontal;
        }

        if (isHorizontal)
        {
            QCoreApplication::sendEvent(QAbstractScrollArea::horizontalScrollBar(), inEvent);
        }
        else
        {
            QCoreApplication::sendEvent(QAbstractScrollArea::verticalScrollBar(), inEvent);
        }
    }
};


#endif //SOLTRANSLATOR_SOLSMOOTHSCROLL_H
