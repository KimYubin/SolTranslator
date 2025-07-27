// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinToolTip.h"

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QToolTip>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QPointer>
#include <QTimer>

#include <qevent.h>

#include "FinUtilibrary.h"


/** 커스텀 툴팁 말풍선 */
class FinToolTipBallon : public QWidget
{
    Q_OBJECT

    // qss에서 사용
    Q_PROPERTY(int triangleBaseWidth READ getTriangleBaseWidth WRITE setTriangleBaseWidth)
    Q_PROPERTY(int triangleHeight READ getTriangleHeight WRITE setTriangleHeight)
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius)
    Q_PROPERTY(float borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)

    static QPointer<FinToolTipBallon> _ins;

public:
    static FinToolTipBallon* instance()
    {
        if (_ins.isNull())
        {
            _ins = new FinToolTipBallon();
        }
        return _ins;
    }

    explicit FinToolTipBallon(QWidget* parent = nullptr);

    void showToolTip(const QWidget* widget);
    void showToolTipImpl(const QWidget* widget);
    void hideTipImmediately();
    void hideTipDelay();

    /**
     * 위젯의 툴팁이 변경될 때 사용합니다.
     * 해당 위젯의 툴팁이 보여지고 있다면, 새로운 툴팁으로 업데이트 합니다.
     */
    void updateWidgetToolTip(const QWidget* inWidget);

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    // property
    int    getTriangleBaseWidth() const;
    void   setTriangleBaseWidth(const int inWidth);
    int    getTriangleHeight() const;
    void   setTriangleHeight(const int inHeight);
    int    getBorderRadius() const;
    void   setBorderRadius(const int inRad);
    float  getBorderWidth() const;
    void   setBorderWidth(const float inWidth);
    int    getSpacing() const;
    void   setSpacing(const int inSpacing);
    QColor getBackgroundColor() const;
    void   setBackgroundColor(const QColor inColor);
    QColor getBorderColor() const;
    void   setBorderColor(const QColor inColor);

    void updateMargins() const;

    QPointer<const QWidget> _currentTargetWidget;

    QLabel* _label;
    QVBoxLayout* _layout;

    int _triangleBaseWidth;
    int _triangleHeight;
    int _borderRadius;
    qreal _borderWidth;

    // 위젯과의 간격
    int _spacing;

    QColor _backgroundColor;
    QColor _borderColor;

    QTimer _expireTimer;
    QTimer _hideTimer;

    enum class ShowDirection
    {
        Top
      , Right
      , Bottom
      , Left
    };

    QMargins triMargins(const ShowDirection inDirection) const;

    // 툴팁의 생성 위치. 타겟 위젯에서 바라보는 방향.
    ShowDirection _direction = ShowDirection::Top;
    QPoint triVertex;
};

QPointer<FinToolTipBallon> FinToolTipBallon::_ins = nullptr;

#include "FinToolTip.moc"



FinToolTipBallon::FinToolTipBallon(QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setObjectName("FinToolTipBallon");
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
    connect(&_expireTimer, &QTimer::timeout, this, &FinToolTipBallon::hideTipImmediately);
    connect(&_hideTimer, &QTimer::timeout, this, &FinToolTipBallon::hideTipImmediately);
}

void FinToolTipBallon::showToolTip(const QWidget* widget)
{
    if (widget && widget->isVisible() && widget->toolTip().isEmpty() == false)
    {
        _currentTargetWidget = QPointer(widget);
        showToolTipImpl(widget);
    }
    else
    {
        hideTipDelay();
    }
}

void FinToolTipBallon::showToolTipImpl(const QWidget* widget)
{
    _label->setText(widget->toolTip());
    _label->adjustSize();
    _label->repaint();
    adjustSize();
    repaint();

    // 라벨 지오메트리 계산
    const QRect labelRect = geometry().marginsRemoved(_layout->contentsMargins());

    // 위치 계산
    const QRect wRect       = widget->rect();
    const QPoint wGlobalPos = widget->mapToGlobal(wRect.topLeft());
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

    const QRect availableGeo = Fin::availableGeometryAt(QCursor::pos());

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
    newRect = Fin::moveToInside(availableGeo, newRect);
    move(newRect.topLeft());

    // 계산된 마진 및 사이즈로 업데이트
    updateMargins();
    adjustSize();
    show();

    _expireTimer.start();
    _hideTimer.stop();
}

void FinToolTipBallon::hideTipImmediately()
{
    close();
    deleteLater();
}

void FinToolTipBallon::hideTipDelay()
{
    if (_hideTimer.isActive() == false)
        _hideTimer.start(300);
}

void FinToolTipBallon::updateWidgetToolTip(const QWidget* inWidget)
{
    if (_currentTargetWidget == inWidget)
    {
        showToolTipImpl(_currentTargetWidget);
    }
}

void FinToolTipBallon::paintEvent(QPaintEvent* inPaintEvent)
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

int FinToolTipBallon::getTriangleBaseWidth() const
{
    return _triangleBaseWidth;
}

void FinToolTipBallon::setTriangleBaseWidth(const int inWidth)
{
    _triangleBaseWidth = inWidth;
    update();
}

int FinToolTipBallon::getTriangleHeight() const
{
    return _triangleHeight;
}

void FinToolTipBallon::setTriangleHeight(const int inHeight)
{
    _triangleHeight = inHeight;
    updateMargins();
    update();
}

int FinToolTipBallon::getBorderRadius() const
{
    return _borderRadius;
}

void FinToolTipBallon::setBorderRadius(const int inRad)
{
    _borderRadius = inRad;
    update();
}

float FinToolTipBallon::getBorderWidth() const
{
    return _borderWidth;
}

void FinToolTipBallon::setBorderWidth(const float inWidth)
{
    _borderWidth = inWidth;
    updateMargins();
    update();
}

int FinToolTipBallon::getSpacing() const
{
    return _spacing;
}

void FinToolTipBallon::setSpacing(const int inSpacing)
{
    _spacing = inSpacing;
    update();
}

QColor FinToolTipBallon::getBackgroundColor() const
{
    return _backgroundColor;
}

void FinToolTipBallon::setBackgroundColor(const QColor inColor)
{
    _backgroundColor = inColor;
    update();
}

QColor FinToolTipBallon::getBorderColor() const
{
    return _borderColor;
}

void FinToolTipBallon::setBorderColor(const QColor inColor)
{
    _borderColor = inColor;
    update();
}

void FinToolTipBallon::updateMargins() const
{
    _layout->setContentsMargins(triMargins(_direction));
}

QMargins FinToolTipBallon::triMargins(const ShowDirection inDirection) const
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

// ~==================================
// FinTooltipFilter 

void FinTooltipFilter::setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip)
{
    static FinTooltipFilter* ins = new FinTooltipFilter();
    inTargetWidget->setToolTip(inToolTip);
    inTargetWidget->installEventFilter(ins);
}

void FinTooltipFilter::setCheckableButtonToolTip(QAbstractButton* inTargetWidget, const QString& inOnCheckToolTip, const QString& inOffCheckToolTip)
{
    QString currentToolTip = inOnCheckToolTip;
    if (inTargetWidget->isCheckable())
    {
        if (inTargetWidget->isChecked())
        {
            currentToolTip = inOffCheckToolTip;
        }
    }

    setBubbleToolTip(inTargetWidget, currentToolTip);

    connect(inTargetWidget, &QAbstractButton::toggled, inTargetWidget, [inTargetWidget, inOnCheckToolTip, inOffCheckToolTip](const bool checked)
    {
        QString toolTip;
        if (checked)
        {
            toolTip = inOffCheckToolTip;
        }
        else
        {
            toolTip = inOnCheckToolTip;
        }
        inTargetWidget->setToolTip(toolTip);

        FinToolTipBallon::instance()->updateWidgetToolTip(inTargetWidget);
    });
}

FinTooltipFilter::FinTooltipFilter(QObject* parent): QObject(parent)
{
}

bool FinTooltipFilter::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::ToolTip:
    {
        // const QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        const QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget == nullptr)
        {
            return false;
        }

        const QString tooltipText = widget->toolTip();
        if (tooltipText.isEmpty() == false)
        {
            FinToolTipBallon::instance()->showToolTip(widget);
        }

        return true; // 기본 툴팁을 차단
    }
    case QEvent::Leave:
    case QEvent::Hide:
    case QEvent::Close:
    case QEvent::Quit:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
    {
        FinToolTipBallon::instance()->hideTipImmediately();
        break;
    }
    default: break;  
    }

    return QObject::eventFilter(obj, event);
}
