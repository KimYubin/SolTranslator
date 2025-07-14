//
// Created by YubinKim on 25/05/27 화.
//

#include "SwitchButton.h"

#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QEasingCurve>

SwitchButton::SwitchButton(QWidget* parent)
    : QCheckBox(parent)
    , _barCheckedBrush(QColor("#00B0FF"))
    , _barUncheckedBrush(Qt::gray)
    , _handleCheckedBrush(Qt::white)
    , _handleUncheckedBrush(Qt::white)
    , _barPen(Qt::transparent)
    , _handleCheckedPen(Qt::transparent)
    , _handleUncheckedPen(Qt::lightGray)
{
    setButtonShape(QSize(46, 28), 1.0f, 20.0f/28.0f);

    _handleAnimation = new QPropertyAnimation(this, "handlePosition", this);
    _handleAnimation->setEasingCurve(QEasingCurve::OutExpo);
    _handleAnimation->setDuration(250);

    _animationGroup = new QSequentialAnimationGroup(this);
    _animationGroup->addAnimation(_handleAnimation);

    connect(this, &QCheckBox::checkStateChanged, this, &SwitchButton::setupAnimation);
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

    const QRectF cntRectF = contentsRect().toRectF();

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
}

float SwitchButton::handlePosition() const
{
    return _handlePos;
}

void SwitchButton::setHandlePosition(const float position)
{
    _handlePos = position;
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

QColor SwitchButton::getBarPenColor() const
{
    return _barPen.color();
}

void SwitchButton::setBarPenColor(const QColor& inColor)
{
    _barPen = inColor;
    update();
}

QColor SwitchButton::getHandleCheckedPenColor() const
{
    return _handleCheckedPen.color();
}

void SwitchButton::setHandleCheckedPenColor(const QColor& inColor)
{
    _handleCheckedPen = inColor;
    update();
}

QColor SwitchButton::getHandleUncheckedPenColor() const
{
    return _handleUncheckedPen.color();
}

void SwitchButton::setHandleUncheckedPenColor(const QColor& inColor)
{
    _handleUncheckedPen = inColor;
    update();
}

