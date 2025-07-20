// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SwitchButton.h"

#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QEasingCurve>

SwitchButton::SwitchButton(const bool inChecked, QWidget* parent)
    : QCheckBox(parent)
    , _barCheckedBrush(QColor(0x00B0FF))
    , _barUncheckedBrush(Qt::gray)
    , _handleCheckedBrush(Qt::white)
    , _handleUncheckedBrush(Qt::white)
    , _barPen(Qt::transparent)
    , _handleCheckedPen(Qt::transparent)
    , _handleUncheckedPen(Qt::lightGray)
    , _handlePos((inChecked ? 1.0f : 0.0f))
{
    setChecked(inChecked);
    setButtonShape(QSize(46, 28), 1.0f, 20.0f/28.0f);

    _handleAnimation = new QPropertyAnimation(this, "handlePosition", this);
    _handleAnimation->setEasingCurve(QEasingCurve::OutExpo);
    _handleAnimation->setDuration(250);

    _animationGroup = new QSequentialAnimationGroup(this);
    _animationGroup->addAnimation(_handleAnimation);

    connect(this, &QCheckBox::checkStateChanged, this, &SwitchButton::setupAnimation);

    setFocusPolicy(Qt::TabFocus);
}

QSize SwitchButton::sizeHint() const
{
    return _size;
}

bool SwitchButton::hitButton(const QPoint& pos) const
{
    return contentsRect().contains(pos);
}

void SwitchButton::setButtonShape(const QSize& inSize, const float inTrackHeightRatio, const float inHandleRatio)
{
    _size             = inSize;
    _trackHeightRatio = inTrackHeightRatio;
    _handleRadRatio   = inHandleRatio / 2.0f;
    update();
}

void SwitchButton::setupAnimation(const Qt::CheckState inCheckState)
{
    _animationGroup->stop();
    _handleAnimation->setEndValue(inCheckState == Qt::Checked ? 1.0f : 0.0f);
    _animationGroup->start();
}

void SwitchButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    // 펜이 그려지기 위해 절반만큼 안쪽으로 들어가서 그려야함. 
    const qreal defaultDrawMargin = qCeil(_barPen.width() / 2.0);
    const QRectF cntRectF = contentsRect().toRectF().marginsRemoved(QMargins(defaultDrawMargin, defaultDrawMargin, defaultDrawMargin, defaultDrawMargin));

    QRectF trackRect(0, 0, cntRectF.width(), cntRectF.height() * _trackHeightRatio);
    trackRect.moveCenter(cntRectF.center());
    const float trackRad = trackRect.height() / 2.0f;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_barPen);

    if (isChecked())
    {
        painter.setBrush(_barCheckedBrush);
        painter.drawRoundedRect(trackRect, trackRad, trackRad);
        painter.setPen(_handleCheckedPen);
        painter.setBrush(_handleCheckedBrush);
    }
    else
    {
        painter.setBrush(_barUncheckedBrush);
        painter.drawRoundedRect(trackRect, trackRad, trackRad);
        painter.setPen(_handleUncheckedPen);
        painter.setBrush(_handleUncheckedBrush);
    }

    const float handleMoveDist = cntRectF.width() - (trackRad * 2); // 핸들이 움직이는 길이
    const float handlePosX     = (handleMoveDist * _handlePos) + cntRectF.x() + trackRad;
    const QPointF handlePoint  = QPointF(handlePosX, trackRect.center().y());
    const float handleRad      = qRound(trackRect.height() * _handleRadRatio);
    painter.drawEllipse(handlePoint, handleRad, handleRad);


    if (hasFocus())
    {
        painter.setPen(_barFocusPen);
        painter.setBrush(Qt::transparent);
        const qreal focusDrawMargin  = qCeil(_barFocusPen.width() / 2.0);
        const QRectF cntFocusRectF = contentsRect().toRectF().marginsRemoved(QMargins(focusDrawMargin, focusDrawMargin, focusDrawMargin, focusDrawMargin));

        QRectF trackFocusRect(0, 0, cntFocusRectF.width(), cntFocusRectF.height());
        trackFocusRect.moveCenter(cntFocusRectF.center());
        const float trackFocusRad = trackFocusRect.height() / 2.0f;
        painter.drawRoundedRect(trackFocusRect, trackFocusRad, trackFocusRad);
    }
}

float SwitchButton::handlePosition() const
{
    return _handlePos;
}

void SwitchButton::setHandlePosition(const float inPosition)
{
    _handlePos = inPosition;
    update();
}

QSize SwitchButton::getSwitchSize() const
{
    return _size;
}

void SwitchButton::setSwitchSize(const QSize& inSize)
{
    _size = inSize;
    update();
}

float SwitchButton::getTrackHeightRatio() const
{
    return _trackHeightRatio;
}

void SwitchButton::setTrackHeightRatio(const float inRatio)
{
    _trackHeightRatio = inRatio;
    update();
}

float SwitchButton::getHandleRatio() const
{
    return _handleRadRatio * 2.0f;
}

void SwitchButton::setHandleRatio(const float inRatio)
{
    _handleRadRatio = (inRatio / 2.0f);
    update();
}

QColor SwitchButton::getBarCheckedColor() const
{
    return _barCheckedBrush.color();
}

void SwitchButton::setBarCheckedColor(const QColor& inColor)
{
    _barCheckedBrush = inColor;
    update();
}

QColor SwitchButton::getBarUncheckedColor() const
{
    return _barUncheckedBrush.color();
}

void SwitchButton::setBarUncheckedColor(const QColor& inColor)
{
    _barUncheckedBrush = inColor;
    update();
}

QColor SwitchButton::getHandleCheckedColor() const
{
    return _handleCheckedBrush.color();
}

void SwitchButton::setHandleCheckedColor(const QColor& inColor)
{
    _handleCheckedBrush = inColor;
    update();
}

QColor SwitchButton::getHandleUncheckedColor() const
{
    return _handleUncheckedBrush.color();
}

void SwitchButton::setHandleUncheckedColor(const QColor& inColor)
{
    _handleUncheckedBrush = inColor;
    update();
}

QColor SwitchButton::getBarBorderColor() const
{
    return _barPen.color();
}

void SwitchButton::setBarBorderColor(const QColor& inColor)
{
    _barPen = inColor;
    update();
}

QColor SwitchButton::getBarFocusBorderColor() const
{
    return _barFocusPen.color();
}

void SwitchButton::setBarFocusBorderColor(const QColor& inColor)
{
    _barFocusPen = inColor;
    update();
}

float SwitchButton::getBarBorderWidth() const
{
    return _barPen.width();
}

void SwitchButton::setBarBorderWidth(const float inWidth)
{
    _barPen.setWidthF(inWidth);
    update();
}

float SwitchButton::getBarFocusBorderWidth() const
{
    return _barFocusPen.width();
}

void SwitchButton::setBarFocusBorderWidth(const float inWidth)
{
    _barFocusPen.setWidthF(inWidth);
    update();
}

QColor SwitchButton::getHandleCheckedBorderColor() const
{
    return _handleCheckedPen.color();
}

void SwitchButton::setHandleCheckedBorderColor(const QColor& inColor)
{
    _handleCheckedPen = inColor;
    update();
}

QColor SwitchButton::getHandleUncheckedBorderColor() const
{
    return _handleUncheckedPen.color();
}

void SwitchButton::setHandleUncheckedBorderColor(const QColor& inColor)
{
    _handleUncheckedPen = inColor;
    update();
}

