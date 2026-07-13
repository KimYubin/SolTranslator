// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHSCROLLAREA_H
#define SOLTRANSLATOR_SOLSMOOTHSCROLLAREA_H

#include <QAbstractScrollArea>
#include <QPointer>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QWheelEvent>

class SolSmoothScrollBar : public QScrollBar
{
public:
    using QScrollBar::QScrollBar;

private:
    bool smoothWheel(QScrollBar* inScrollBar, QPointer<QPropertyAnimation>& inAnim, const float inAngleDelta)
    {
        if (inAnim.isNull())
        {
            inAnim = new QPropertyAnimation(inScrollBar, "value", this);
            QObject::connect(inScrollBar, &QScrollBar::rangeChanged, inAnim, [inAnim](int min, int max)
            {
                if (min == max)
                {
                    inAnim->stop();
                }
            });
        }

        const int curVal = inScrollBar->value();
        if (inAnim->state() != QAbstractAnimation::Running)
        {
            _targetValue = curVal;
        }
        else
        {
            inAnim->stop();
        }

        const float deltaStep = inAngleDelta / 120.f;
        const int deltaVal    = deltaStep * inScrollBar->singleStep();

        if ((deltaStep < 0 && curVal < _targetValue) || (deltaStep > 0 && curVal > _targetValue))
        {
            _targetValue = _targetValue - deltaVal;
        }
        else
        {
            // 반대방향으로 이동
            _targetValue = curVal - deltaVal;
        }

        const int minVal = inScrollBar->minimum();
        const int maxVal = inScrollBar->maximum();
        _targetValue     = std::clamp(_targetValue, minVal, maxVal);
        if (curVal == _targetValue)
        {
            return false;
        }

        inAnim->setDuration(200); // 애니메이션 지속 시간 (ms)
        inAnim->setStartValue(curVal);
        inAnim->setEndValue(_targetValue);
        inAnim->setEasingCurve(QEasingCurve::OutCubic);
        inAnim->start();

        return true;
    }

protected:
    virtual void wheelEvent(QWheelEvent* event) override
    {
        const QPoint angleDelta = event->angleDelta();

        event->ignore();
        bool horizontal = qAbs(angleDelta.x()) > qAbs(angleDelta.y());

        if (!horizontal && angleDelta.x() != 0 && orientation() == Qt::Horizontal)
        {
            return;
        }

        const int delta = horizontal ? -angleDelta.x() : angleDelta.y();

        smoothWheel(this, _scrollAnim, delta);

        if (smoothWheel(this, _scrollAnim, delta))
        {
            event->accept();
        }
    }

    QPointer<QPropertyAnimation> _scrollAnim;
    int _targetValue = 0;
};

template <typename BaseScrollArea>
class SolSmoothScrollArea : public BaseScrollArea
{
public:
    using Base = BaseScrollArea;
    // using Base::Base;

    explicit SolSmoothScrollArea(QWidget *parent = nullptr)
        :Base(parent)
    {
        QAbstractScrollArea::setHorizontalScrollBar(new SolSmoothScrollBar);
        QAbstractScrollArea::setVerticalScrollBar(new SolSmoothScrollBar);
    };
private:
    void smoothWheel(QScrollBar* inScrollBar, QPointer<QPropertyAnimation>& inAnim, const float inAngleDelta)
    {
        if (inAnim.isNull())
        {
            inAnim = new QPropertyAnimation(inScrollBar, "value", this);
            QObject::connect(inScrollBar, &QScrollBar::rangeChanged, inAnim, [inAnim](int min, int max)
            {
                if (min == max)
                {
                    inAnim->stop();
                }
            });
        }

        const int curVal = inScrollBar->value();
        if (inAnim->state() != QAbstractAnimation::Running)
        {
            _targetValue = curVal;
        }
        else
        {
            inAnim->stop();
        }

        const float deltaStep = inAngleDelta / 120.f;
        const int deltaVal    = deltaStep * inScrollBar->singleStep();

        if ((deltaStep < 0 && curVal < _targetValue) || (deltaStep > 0 && curVal > _targetValue))
        {
            _targetValue = _targetValue - deltaVal;
        }
        else
        {
            // 반대방향으로 이동
            _targetValue = curVal - deltaVal;
        }

        const int minVal = inScrollBar->minimum();
        const int maxVal = inScrollBar->maximum();
        _targetValue     = std::clamp(_targetValue, minVal, maxVal);

        inAnim->setDuration(200); // 애니메이션 지속 시간 (ms)
        inAnim->setStartValue(curVal);
        inAnim->setEndValue(_targetValue);
        inAnim->setEasingCurve(QEasingCurve::OutCubic);
        inAnim->start();
    }

protected:
    virtual void wheelEvent(QWheelEvent* event) override
    {
        const QPoint angleDelta = event->angleDelta();

        // Smooth Scroll
        if (angleDelta.x() != 0)
        {
            smoothWheel(QAbstractScrollArea::horizontalScrollBar(), _hScrollAnim, angleDelta.x());
        }
        if (angleDelta.y() != 0)
        {
            smoothWheel(QAbstractScrollArea::verticalScrollBar(), _vScrollAnim, angleDelta.y());
        }

        event->accept();

        Base::updateMicroFocus();
    }

    QPointer<QPropertyAnimation> _hScrollAnim;
    QPointer<QPropertyAnimation> _vScrollAnim;
    int _targetValue = 0;
};


#endif //SOLTRANSLATOR_SOLSMOOTHSCROLLAREA_H
