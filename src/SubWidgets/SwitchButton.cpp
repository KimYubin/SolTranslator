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
    , _barCheckedBrush(QColor("#00B0FF").lighter())
    , _handleBrush(Qt::white)
    , _handleCheckedBrush(QColor("#00B0FF"))
    , _pulseUncheckedBrush(QColor("#44999999"))
    , _pulseCheckedBrush(QColor("#4400B0EE"))
    , _transparentPen(Qt::transparent)
    , _lightGreyPen(Qt::lightGray)
{
    setContentsMargins(8, 0, 8, 0);

    _handleAnimation = new QPropertyAnimation(this, "handlePosition", this);
    _handleAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    _handleAnimation->setDuration(200);

    _pulseAnimation = new QPropertyAnimation(this, "pulseRadius", this);
    _pulseAnimation->setDuration(350);
    _pulseAnimation->setStartValue(10);
    _pulseAnimation->setEndValue(20);

    _animationGroup = new QSequentialAnimationGroup(this);
    _animationGroup->addAnimation(_handleAnimation);
    _animationGroup->addAnimation(_pulseAnimation);

    connect(this, &QCheckBox::checkStateChanged, this, &SwitchButton::setupAnimation);
}

QSize SwitchButton::sizeHint() const
{
    return QSize(58, 45);
}

bool SwitchButton::hitButton(const QPoint& pos) const
{
    return contentsRect().contains(pos);
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

    const QRect contentRect = contentsRect();
    const int handleRadius  = qRound(0.24 * contentRect.height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_transparentPen);

    QRectF barRect(0, 0, contentRect.width() - handleRadius, 0.40 * contentRect.height());
    barRect.moveCenter(contentRect.center().toPointF());

    const float rounding    = barRect.height() / 2.0f;
    const float trailLength = contentRect.width() - 2 * handleRadius;

    const float xPos = contentRect.x() + handleRadius + trailLength * _handlePos;

    if (_pulseAnimation->state() == QAbstractAnimation::Running)
    {
        painter.setBrush(isChecked() ? _pulseCheckedBrush : _pulseUncheckedBrush);
        painter.drawEllipse(QPointF(xPos, barRect.center().y()), _pulseRad, _pulseRad);
    }

    if (isChecked())
    {
        painter.setBrush(_barCheckedBrush);
        painter.drawRoundedRect(barRect, rounding, rounding);
        painter.setBrush(_handleCheckedBrush);
    }
    else
    {
        painter.setBrush(_barBrush);
        painter.drawRoundedRect(barRect, rounding, rounding);
        painter.setPen(_lightGreyPen);
        painter.setBrush(_handleBrush);
    }

    painter.drawEllipse(QPointF(xPos, barRect.center().y()), handleRadius, handleRadius);
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

float SwitchButton::pulseRadius() const
{
    return _pulseRad;
}

void SwitchButton::setPulseRadius(const float radius)
{
    _pulseRad = radius;
    update();
}
