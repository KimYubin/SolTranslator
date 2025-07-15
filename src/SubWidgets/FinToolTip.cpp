// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinToolTip.h"

#include <QLabel>
#include <QWidget>
#include <QToolTip>

#include <qevent.h>


/** 커스텀 툴팁 말풍선 */
class FinToolTipBallon : public QWidget
{
    Q_OBJECT

public:
    static FinToolTipBallon* instance()
    {
        static FinToolTipBallon* ins = new FinToolTipBallon();
        return ins;
    }
    
    explicit FinToolTipBallon(QWidget* parent = nullptr);

    void showToolTip(const QString& inText, const QPoint& inPos);

private:
    QLabel* _label;
};

#include "FinToolTip.moc"



FinToolTipBallon::FinToolTipBallon(QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setObjectName("FinToolTipBallon");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    _label = new QLabel(this);
}

void FinToolTipBallon::showToolTip(const QString& inText, const QPoint& inPos)
{
    _label->setText(inText);
    _label->adjustSize();
    resize(_label->size());

    QPoint newPos = {inPos.x() - (_label->width() / 2), inPos.y() - _label->height()};
    move(newPos);
    show();
}


// ~==================================
// FinTooltipFilter 

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
            const QPoint globalPos = widget->mapToGlobal(QPoint(0, 0)) + QPoint(0, -5);
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
