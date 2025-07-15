// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTOOLTIP_H
#define FINTOOLTIP_H

#include <QObject>


class FinToolTipBallon;

/**
 * 커스텀 툴팁을 표시하기 위한 이벤트 필터 객체입니다.
 *
 * widget->setToolTip("tool tip"));
 * widget->installEventFilter(new FinTooltipFilter(qApp));
 */
class FinTooltipFilter : public QObject
{
    Q_OBJECT

public:
    explicit FinTooltipFilter(QObject* parent = nullptr);

    virtual bool eventFilter(QObject* obj, QEvent* event) override;
};


#endif //FINTOOLTIP_H
