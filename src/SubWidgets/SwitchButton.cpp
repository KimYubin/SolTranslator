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
    , _transparentPen(Qt::transparent)
    , _lightGreyPen(Qt::lightGray)
{
    setContentsMargins(8, 0, 8, 0);

    _handleAnimation = new QPropertyAnimation(this, "handlePosition", this);
    _handleAnimation->setEasingCurve(QEasingCurve::OutExpo);
    _handleAnimation->setDuration(300);

    _animationGroup = new QSequentialAnimationGroup(this);
    _animationGroup->addAnimation(_handleAnimation);

    connect(this, &QCheckBox::checkStateChanged, this, &SwitchButton::setupAnimation);
}

QSize SwitchButton::sizeHint() const
{
    // w, 22
    return QSize(58, 40);
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

    const float handleRadRatio  = 0.24f;
    const float trackHightRatio = 0.55f;

    const QRectF cntRectF = contentsRect().toRectF();
    const float handleRad = qRound(handleRadRatio * cntRectF.height());

    QRectF trackRect(0, 0, cntRectF.width() - handleRad, cntRectF.height() * trackHightRatio);
    const float trackRounding = trackRect.height() / 2.0f;

    const float handleRailLength = (handleRad * 2) - cntRectF.width(); // 핸들이 움직일 거리
    const float handleXPos       = (handleRailLength * _handlePos) + cntRectF.x() + handleRad;
    const QPointF handlePoint    = QPointF(handleXPos, trackRect.center().y());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_transparentPen);

    if (isChecked())
    {
        painter.setBrush(_barCheckedBrush);
        painter.drawRoundedRect(trackRect, trackRounding, trackRounding);
        painter.setBrush(_handleCheckedBrush);
    }
    else
    {
        painter.setBrush(_barBrush);
        painter.drawRoundedRect(trackRect, trackRounding, trackRounding);
        painter.setPen(_lightGreyPen);
        painter.setBrush(_handleBrush);
    }

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
