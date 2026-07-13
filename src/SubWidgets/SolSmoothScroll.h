// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHSCROLL_H
#define SOLTRANSLATOR_SOLSMOOTHSCROLL_H

#include <QAbstractScrollArea>
#include <QPointer>
#include <QScrollBar>


class QPropertyAnimation;

/**
 * The SolScrollSmoothComponent class is a component class
 * that provides a smooth wheel scrolling feature.
 */
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

/**
 * The SolSmoothScrollBar class is a QScrollBar-derived class
 * that provides smooth wheel scrolling.
 */
class SolSmoothScrollBar : public QScrollBar
{
public:
    explicit SolSmoothScrollBar(QWidget* inParent = nullptr);
    bool setSmoothValue(const float inAngleDelta);

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

    SolScrollSmoothComponent* _smoothComponent;
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

};


#endif //SOLTRANSLATOR_SOLSMOOTHSCROLL_H
