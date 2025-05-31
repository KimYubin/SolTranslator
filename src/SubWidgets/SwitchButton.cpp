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
    , _barBrush(Qt::gray)
    , _barCheckedBrush(QColor("#00B0FF"))
    , _handleBrush(Qt::white)
    , _transparentPen(Qt::transparent)
    , _lightGreyPen(Qt::lightGray)
{
    setButtonShape(QSize(40, 22), 1.0f, 0.75f);

    _handleAnimation = new QPropertyAnimation(this, "handlePosition", this);
    _handleAnimation->setEasingCurve(QEasingCurve::OutExpo);
    _handleAnimation->setDuration(300);

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
    painter.setPen(_transparentPen);

    if (isChecked())
    {
        painter.setBrush(_barCheckedBrush);
        painter.drawRoundedRect(trackRect, trackRad, trackRad);
        painter.setBrush(_handleBrush);
    }
    else
    {
        painter.setBrush(_barBrush);
        painter.drawRoundedRect(trackRect, trackRad, trackRad);
        painter.setPen(_lightGreyPen);
        painter.setBrush(_handleBrush);
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
