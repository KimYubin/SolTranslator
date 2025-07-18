// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTOOLTIP_H
#define FINTOOLTIP_H

#include <QObject>


class FinToolTipBallon;

/**
 * 커스텀 툴팁을 표시하기 위한 이벤트 필터 객체입니다.
 *
 * @see FinTooltipFilter::setBubbleToolTip(widget, "tool tip");
 */
class FinTooltipFilter : public QObject
{
    Q_OBJECT

public:
    /**
     * 버블 툴팁을 추가합니다.
     * 
     * @param inTargetWidget 툴팁을 추가할 위젯 
     * @param inToolTip 툴팁 내용
     */
    static void setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip);

    explicit FinTooltipFilter(QObject* parent = nullptr);

    virtual bool eventFilter(QObject* obj, QEvent* event) override;
};


#endif //FINTOOLTIP_H
