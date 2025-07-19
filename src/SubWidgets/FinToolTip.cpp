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
    QColor getBackgroundColor() const;
    void   setBackgroundColor(const QColor inColor);
    QColor getBorderColor() const;
    void   setBorderColor(const QColor inColor);

    void updateMargins() const;

    QLabel* _label;
    QVBoxLayout* _layout;

    int _triangleBaseWidth;
    int _triangleHeight;
    int _borderRadius;
    qreal _borderWidth;

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

     ShowDirection _direction = ShowDirection::Top;
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
    _label->repaint(); // 이전 문자열 깜빡임 방지
    adjustSize();
    repaint();

    // 위치 계산
    const QRect wRect       = widget->rect();
    const QPoint wGlobalPos = widget->mapToGlobal(wRect.topLeft());
    const QPoint wCenterPos = {wGlobalPos.x() + (wRect.width() / 2), wGlobalPos.y() + (wRect.height() / 2)};

    // 상하, 좌우 각각 공유하는 중앙 위치
    const int topBottomX = wCenterPos.x() - (width() / 2);
    const int lefRightY  = wCenterPos.y() - (height() / 2);

    constexpr int interval = 2;
    const int topY    = wGlobalPos.y() - height() - interval;
    const int bottomY = wGlobalPos.y() + wRect.height() + interval;
    const int rightX  = wGlobalPos.x() + wRect.width() + interval;
    const int leftX   = wGlobalPos.x() - width() - interval;

    const QPoint newTopPos    = {topBottomX, topY};
    const QPoint newRightPos  = {rightX, lefRightY};
    const QPoint newBottomPos = {topBottomX, bottomY};
    const QPoint newLeftPos   = {leftX, lefRightY};

    const QRect availableGeo = Fin::availableGeometryAt(QCursor::pos());

    // 교집합 면적 최대값 계산하고, _direction을 업데이트합니다.
    // 겹치는 면적이 가장 넓은 방향으로 생성합니다.
    auto checkMaxArea = [availableGeo, this](int inMaxArea, const ShowDirection inNewSD, const QPoint& inNewPos)
    {
        auto availGeoInterArea = [availableGeo](const QRect& inRect)
        {
            const QRect intersection  = availableGeo & inRect;
            const int intersectedArea = intersection.width() * intersection.height();
            return intersectedArea;
        };

        QRect currentGeo = frameGeometry();
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
    maxArea = checkMaxArea(maxArea, ShowDirection::Right, newRightPos);
    maxArea = checkMaxArea(maxArea, ShowDirection::Bottom, newBottomPos);
    maxArea = checkMaxArea(maxArea, ShowDirection::Left, newLeftPos);

    QRect current = frameGeometry();

    // 가장 적절한 방향으로 이동
    switch (_direction)
    {
    case ShowDirection::Top:
        current.moveTo(newTopPos);
        break;
    case ShowDirection::Right:
        current.moveTo(newRightPos);
        break;
    case ShowDirection::Bottom:
        current.moveTo(newBottomPos);
        break;
    case ShowDirection::Left:
        current.moveTo(newLeftPos);
        break;
    }

    // 벗어나면 안쪽으로 이동
    current = Fin::moveToInside(availableGeo, current);

    move(current.topLeft());

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

void FinToolTipBallon::paintEvent(QPaintEvent* inPaintEvent)
{
    QWidget::paintEvent(inPaintEvent);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(_backgroundColor);

    const QPen qPen(_borderColor, _borderWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(qPen);

    const double halfWidth        = width() / 2.0;
    const double halfTriBaseWidth = _triangleBaseWidth / 2.0;
    const QMargins layoutMargin   = _layout->contentsMargins();
    const QRect lineRect          = rect() - layoutMargin; // 하단 꼬리, 테두리 공간 확보


    QPainterPath path;

    // 좌상단
    path.moveTo(lineRect.left() + _borderRadius, lineRect.top());

    // 우상단, 둥근 모서리
    path.lineTo(lineRect.right() - _borderRadius, lineRect.top());
    path.quadTo(lineRect.right(), lineRect.top(), lineRect.right(), lineRect.top() + _borderRadius);

    // 우하단, 둥근 모서리
    path.lineTo(lineRect.right(), lineRect.bottom() - _borderRadius);
    path.quadTo(lineRect.right(), lineRect.bottom(), lineRect.right() - _borderRadius, lineRect.bottom());

    // 말풍선 꼬리 삼각형
    path.lineTo(halfWidth + halfTriBaseWidth, lineRect.bottom());
    path.lineTo(halfWidth, height());
    path.lineTo(halfWidth - 1, height());
    path.lineTo(halfWidth - 1 - halfTriBaseWidth, lineRect.bottom());

    // 좌하단, 둥근 모서리
    path.lineTo(lineRect.left() + _borderRadius, lineRect.bottom());
    path.quadTo(lineRect.left(), lineRect.bottom(), lineRect.left(), lineRect.bottom() - _borderRadius);

    // 좌상단, 둥근 모서리
    path.lineTo(lineRect.left(), lineRect.top() + _borderRadius);
    path.quadTo(lineRect.left(), lineRect.top(), lineRect.left() + _borderRadius, lineRect.top());


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
}

void FinToolTipBallon::updateMargins() const
{
    const int borderMargin = qCeil(_borderWidth);
    _layout->setContentsMargins(borderMargin, borderMargin, borderMargin, _triangleHeight);
}

// ~==================================
// FinTooltipFilter 

void FinTooltipFilter::setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip)
{
    inTargetWidget->setToolTip(inToolTip);
    inTargetWidget->installEventFilter(new FinTooltipFilter(inTargetWidget));
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
        FinToolTipBallon::instance()->showToolTip(inTargetWidget);
    });
}

FinTooltipFilter::FinTooltipFilter(QObject* parent): QObject(parent)
{
}

bool FinTooltipFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (/*event->type() == QEvent::Enter ||*/ event->type() == QEvent::ToolTip)
    {
        const QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
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
    else if (event->type() == QEvent::Leave
        || event->type() == QEvent::Hide
        || event->type() == QEvent::Close
        || event->type() == QEvent::Quit)
    {
        FinToolTipBallon::instance()->hideTipImmediately();
    }

    return QObject::eventFilter(obj, event);
}
