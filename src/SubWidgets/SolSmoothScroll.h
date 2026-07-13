// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHSCROLL_H
#define SOLTRANSLATOR_SOLSMOOTHSCROLL_H

#include <QAbstractScrollArea>
#include <QPointer>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QWheelEvent>


class SolScrollSmoothComponent : public QObject
{
public:
    explicit SolScrollSmoothComponent(QWidget* inParent, QScrollBar* inScrollBar);
    void setScrollBar(QScrollBar* inScrollBar);

    bool smoothWheel(const float inAngleDelta);

    QPointer<QScrollBar> _scrollBar;
    QPropertyAnimation* _scrollAnim;
    int _targetValue = 0;
};


class SolSmoothScrollBar : public QScrollBar
{
public:
    explicit SolSmoothScrollBar(QWidget* inParent = nullptr);
    bool setSmoothValue(const float inAngleDelta);

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

    SolScrollSmoothComponent* _smoothComponent;
};


template <typename BaseType>
class SolSmoothAbstractScrollArea : public BaseType
{
public:
    using Base = BaseType;

    explicit SolSmoothAbstractScrollArea(QWidget* inParent = nullptr)
        : Base(inParent)
    {
        _hScrollBar = new SolSmoothScrollBar(this);
        _vScrollBar = new SolSmoothScrollBar(this);
        Base::setHorizontalScrollBar(_hScrollBar);
        Base::setVerticalScrollBar(_vScrollBar);
        _hSmoothComponent = new SolScrollSmoothComponent{this, _hScrollBar};
        _vSmoothComponent = new SolScrollSmoothComponent{this, _vScrollBar};
    };

protected:
    virtual void wheelEvent(QWheelEvent* event) override
    {
        const QPoint angleDelta = event->angleDelta();

        // Smooth Scroll
        if (angleDelta.x() != 0)
        {
            if (_hScrollBar)
            {
                _hScrollBar->setSmoothValue(angleDelta.x());
            }
            else
            {
                Base::wheelEvent(event);
                return;
            }
            // _hSmoothComponent->smoothWheel(angleDelta.x());
        }
        if (angleDelta.y() != 0)
        {
            if (_vScrollBar)
            {
                _vScrollBar->setSmoothValue(angleDelta.y());
            }
            else
            {
                Base::wheelEvent(event);
                return;
            }
            // _vSmoothComponent->smoothWheel(angleDelta.y());
        }

        event->accept();

        Base::updateMicroFocus();
    }


    QPointer<SolSmoothScrollBar> _hScrollBar;
    QPointer<SolSmoothScrollBar> _vScrollBar;

    SolScrollSmoothComponent* _hSmoothComponent;
    SolScrollSmoothComponent* _vSmoothComponent;
};


#endif //SOLTRANSLATOR_SOLSMOOTHSCROLL_H
