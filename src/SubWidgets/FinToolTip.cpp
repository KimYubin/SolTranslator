// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinToolTip.h"

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QToolTip>
#include <QVBoxLayout>

#include <qevent.h>


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

public:
    static FinToolTipBallon* instance()
    {
        static FinToolTipBallon* ins = new FinToolTipBallon();
        return ins;
    }

    explicit FinToolTipBallon(QWidget* parent = nullptr);

    void showToolTip(const QString& inText, const QPoint& inPos);

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
};

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
}

void FinToolTipBallon::showToolTip(const QString& inText, const QPoint& inPos)
{
    _label->setText(inText);
    _label->adjustSize();
    adjustSize();

    const QPoint newPos = {inPos.x() - (width() / 2), inPos.y() - height()};
    move(newPos);
    show();
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

FinTooltipFilter::FinTooltipFilter(QObject* parent): QObject(parent)
{
}

bool FinTooltipFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::ToolTip)
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
            // 중앙 상단
            const QSize widgetSize = widget->size();
            const QPoint globalPos = widget->mapToGlobal(QPoint(0, 0));
            const QPoint centerPos = {globalPos.x() + (widgetSize.width() / 2), globalPos.y()};

            FinToolTipBallon::instance()->showToolTip(tooltipText, centerPos);

            return true; // 기본 툴팁을 차단
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        FinToolTipBallon::instance()->hide();
    }

    return QObject::eventFilter(obj, event);
}
