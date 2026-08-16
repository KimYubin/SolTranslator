// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolToolTipBallon.h"

#include "SolToolTip.h"
#include "Types/ToolTipData.h"
#include "Utils/SolDebug.h"
#include "Utils/SolUtilibrary.h"

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QTimer>
#include <QVBoxLayout>
#include <qevent.h>

QPointer<SolToolTipBallon> SolToolTipBallon::_ins = nullptr;


SolToolTipBallon::SolToolTipBallon(QWidget* inParent)
    : QWidget(inParent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setObjectName("SolToolTipBallon");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    _triangleBaseWidth = 8;
    _triangleHeight    = 4;
    _borderRadius      = 4;
    _borderWidth       = 0.5f;
    _spacing           = 0;

    _backgroundColor = QColor(0, 0, 150, 230);
    _borderColor     = QColor(255, 255, 255, 230);


    _label  = new QLabel(this);

    _layout = new QVBoxLayout(this);
    updateMargins();
    _layout->addWidget(_label);


    _expireTimer.setInterval(60'000);
    _expireTimer.setSingleShot(true);
    _hideTimer.setInterval(300);
    _hideTimer.setSingleShot(true);
    connect(&_expireTimer, &QTimer::timeout, this, &SolToolTipBallon::hideToolTipImmediately);
    connect(&_hideTimer, &QTimer::timeout, this, &SolToolTipBallon::hideToolTipImmediately);
}

void SolToolTipBallon::showToolTip(const QWidget* inWidget)
{
    if (inWidget && inWidget->isVisible() && SolToolTip::isValidToolTip(inWidget))
    {
        _currentTargetWidget = QPointer{inWidget};
        showToolTipImpl();
    }
    else
    {
        hideToolTipDelay();
    }
}

void SolToolTipBallon::hideToolTipImmediately()
{
    // Avoid access after calling deleteLater()
    _ins = nullptr;

    close();
    deleteLater();
}

void SolToolTipBallon::updateWidgetToolTip(const QWidget* inWidget)
{
    if (inWidget && _currentTargetWidget == inWidget)
    {
        showToolTipImpl();
    }
}

void SolToolTipBallon::showToolTipImpl()
{
    if (_currentTargetWidget.isNull())
    {
        return;
    }

    const QWidget* curWidget = _currentTargetWidget.get();

    const ToolTipData toolTipData = SolToolTip::getToolTipData(curWidget);
    const QString tooltipString   = toolTipData.toolTipShortcutString();
    if (tooltipString.isEmpty())
    {
        return;
    }

    _label->setText(tooltipString);
    _label->adjustSize();
    _label->repaint();
    adjustSize();
    repaint();

    // 라벨 지오메트리 계산
    const QRect labelRect = geometry().marginsRemoved(_layout->contentsMargins());

    // 위치 계산
    const QRect wRect       = curWidget->rect();
    const QPoint wGlobalPos = curWidget->mapToGlobal(wRect.topLeft());
    const QPoint wCenterPos = {wGlobalPos.x() + (wRect.width() / 2), wGlobalPos.y() + (wRect.height() / 2)};

    // 상하, 좌우 각각 공유하는 중앙 위치
    // 라벨 크기 + 각 시나리오별 여백 추가
    const int topBottomX = wCenterPos.x() - ((labelRect.width() / 2) + _borderWidth);
    const int lefRightY  = wCenterPos.y() - ((labelRect.height() / 2) + _borderWidth);

    const int topY    = wGlobalPos.y() - labelRect.height() - _spacing - (_triangleHeight + _borderWidth); // 위젯 방향 마진 반영
    const int bottomY = wGlobalPos.y() + wRect.height()     + _spacing;
    const int rightX  = wGlobalPos.x() + wRect.width()      + _spacing;
    const int leftX   = wGlobalPos.x() - labelRect.width()  - _spacing - (_triangleHeight + _borderWidth);

    const QPoint newTopPos    = {topBottomX, topY};
    const QPoint newRightPos  = {rightX, lefRightY};
    const QPoint newBottomPos = {topBottomX, bottomY};
    const QPoint newLeftPos   = {leftX, lefRightY};

    const QRect availableGeo = Sol::availableGeometryAt(QCursor::pos());

    // 교집합 면적 최대값 계산하고, _direction을 업데이트합니다.
    // 겹치는 면적이 가장 넓은 방향으로 생성합니다.
    auto checkMaxArea = [availableGeo, labelRect, this](int inMaxArea, const ShowDirection inNewSD, const QPoint& inNewPos)
    {
        auto availGeoInterArea = [availableGeo](const QRect& inRect)
        {
            const QRect intersection  = availableGeo & inRect;
            const int intersectedArea = intersection.width() * intersection.height();
            return intersectedArea;
        };

        QRect currentGeo = labelRect;
        currentGeo.moveTo(inNewPos);
        const int newInterArea = availGeoInterArea(currentGeo);
        if (inMaxArea < newInterArea)
        {
            inMaxArea  = newInterArea;
            _direction = inNewSD;
        }
        return inMaxArea;
    };

    int maxArea = 0;
    _direction  = ShowDirection::Top;

    maxArea = checkMaxArea(maxArea, ShowDirection::Top, newTopPos);
    maxArea = checkMaxArea(maxArea, ShowDirection::Bottom, newBottomPos);
    maxArea = checkMaxArea(maxArea, ShowDirection::Left, newLeftPos);
    maxArea = checkMaxArea(maxArea, ShowDirection::Right, newRightPos);

    QRect newRect = labelRect;
    switch (_direction)
    {
    case ShowDirection::Top:
        newRect.moveTo(newTopPos);
        break;
    case ShowDirection::Right:
        newRect.moveTo(newRightPos);
        break;
    case ShowDirection::Bottom:
        newRect.moveTo(newBottomPos);
        break;
    case ShowDirection::Left:
        newRect.moveTo(newLeftPos);
        break;
    }

    // 벗어나면 안쪽으로 이동
    newRect = Sol::moveToInside(availableGeo, newRect);
    move(newRect.topLeft());

    // 계산된 마진 및 사이즈로 업데이트
    updateMargins();
    adjustSize();
    show();

    // To draw on the always-on-top widget.
    QTimer::singleShot(0, this, &QWidget::raise);

    _expireTimer.start();
    _hideTimer.stop();
}

void SolToolTipBallon::hideToolTipDelay()
{
    if (_hideTimer.isActive() == false)
    {
        _hideTimer.start(300);
    }
}

void SolToolTipBallon::paintEvent(QPaintEvent* inPaintEvent)
{
    QWidget::paintEvent(inPaintEvent);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(_backgroundColor);

    const QPen qPen(_borderColor, _borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(qPen);

    const double halfWidth        = qFloor(width() / 2.0);
    const double halfHeight       = qFloor(height() / 2.0);
    const double halfTriBaseWidth = _triangleBaseWidth / 2.0;
    const QMargins layoutMargin   = _layout->contentsMargins();
    const QRect lineRect          = rect() - layoutMargin - QMargins(1, 1, 0, 0); // 하단 꼬리, 테두리 공간 확보

    constexpr double vertexWidth = 0.25;

    QPainterPath path;

    // 좌상단
    path.moveTo(lineRect.left() + _borderRadius, lineRect.top());

    if (_direction == ShowDirection::Bottom)
    {
        path.lineTo(halfWidth - vertexWidth - halfTriBaseWidth, lineRect.top());
        path.lineTo(halfWidth - vertexWidth, 0);
        path.lineTo(halfWidth + vertexWidth, 0);
        path.lineTo(halfWidth + vertexWidth + halfTriBaseWidth, lineRect.top());
    }

    // 우상단, 둥근 모서리
    path.lineTo(lineRect.right() - _borderRadius, lineRect.top());
    path.quadTo(lineRect.right(), lineRect.top()
              , lineRect.right(), lineRect.top() + _borderRadius);

    if (_direction == ShowDirection::Left)
    {
        path.lineTo(lineRect.right(), halfHeight - vertexWidth - halfTriBaseWidth);
        path.lineTo(width(), halfHeight - vertexWidth);
        path.lineTo(width(), halfHeight + vertexWidth);
        path.lineTo(lineRect.right(), halfHeight + vertexWidth + halfTriBaseWidth);
    }

    // 우하단, 둥근 모서리
    path.lineTo(lineRect.right(), lineRect.bottom() - _borderRadius);
    path.quadTo(lineRect.right(), lineRect.bottom()
              , lineRect.right() - _borderRadius, lineRect.bottom());

    if (_direction == ShowDirection::Top)
    {
        path.lineTo(halfWidth + vertexWidth + halfTriBaseWidth, lineRect.bottom());
        path.lineTo(halfWidth + vertexWidth, height());
        path.lineTo(halfWidth - vertexWidth, height());
        path.lineTo(halfWidth - vertexWidth - halfTriBaseWidth, lineRect.bottom());
    }

    // 좌하단, 둥근 모서리
    path.lineTo(lineRect.left() + _borderRadius, lineRect.bottom());
    path.quadTo(lineRect.left(), lineRect.bottom()
              , lineRect.left(), lineRect.bottom() - _borderRadius);

    if (_direction == ShowDirection::Right)
    {
        path.lineTo(lineRect.left(), halfHeight + vertexWidth + halfTriBaseWidth);
        path.lineTo(0, halfHeight + vertexWidth);
        path.lineTo(0, halfHeight - vertexWidth);
        path.lineTo(lineRect.left(), halfHeight - vertexWidth - halfTriBaseWidth);
    }

    // 좌상단, 둥근 모서리
    path.lineTo(lineRect.left(), lineRect.top() + _borderRadius);
    path.quadTo(lineRect.left(), lineRect.top()
              , lineRect.left() + _borderRadius, lineRect.top());


    path.closeSubpath();

    painter.drawPath(path);
}

int SolToolTipBallon::getTriangleBaseWidth() const
{
    return _triangleBaseWidth;
}

void SolToolTipBallon::setTriangleBaseWidth(const int inWidth)
{
    _triangleBaseWidth = inWidth;
    update();
}

int SolToolTipBallon::getTriangleHeight() const
{
    return _triangleHeight;
}

void SolToolTipBallon::setTriangleHeight(const int inHeight)
{
    _triangleHeight = inHeight;
    updateMargins();
    update();
}

int SolToolTipBallon::getBorderRadius() const
{
    return _borderRadius;
}

void SolToolTipBallon::setBorderRadius(const int inRad)
{
    _borderRadius = inRad;
    update();
}

float SolToolTipBallon::getBorderWidth() const
{
    return _borderWidth;
}

void SolToolTipBallon::setBorderWidth(const float inWidth)
{
    _borderWidth = inWidth;
    updateMargins();
    update();
}

int SolToolTipBallon::getSpacing() const
{
    return _spacing;
}

void SolToolTipBallon::setSpacing(const int inSpacing)
{
    _spacing = inSpacing;
    update();
}

QColor SolToolTipBallon::getBackgroundColor() const
{
    return _backgroundColor;
}

void SolToolTipBallon::setBackgroundColor(const QColor inColor)
{
    _backgroundColor = inColor;
    update();
}

QColor SolToolTipBallon::getBorderColor() const
{
    return _borderColor;
}

void SolToolTipBallon::setBorderColor(const QColor inColor)
{
    _borderColor = inColor;
    update();
}

void SolToolTipBallon::updateMargins() const
{
    _layout->setContentsMargins(triMargins(_direction));
}

QMargins SolToolTipBallon::triMargins(const ShowDirection inDirection) const
{
    const int borderMargin = qCeil(_borderWidth);

    QMargins margins = {borderMargin, borderMargin, borderMargin, _triangleHeight};
    switch (inDirection)
    {
    case ShowDirection::Top:
        margins = {borderMargin, borderMargin, borderMargin, _triangleHeight};
        break;
    case ShowDirection::Right:
        margins = {_triangleHeight, borderMargin, borderMargin, borderMargin};
        break;
    case ShowDirection::Bottom:
        margins = {borderMargin, _triangleHeight, borderMargin, borderMargin};
        break;
    case ShowDirection::Left:
        margins = {borderMargin, borderMargin, _triangleHeight, borderMargin};
        break;
    }
    return margins;
}
