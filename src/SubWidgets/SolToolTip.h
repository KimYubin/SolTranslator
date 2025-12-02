// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLTOOLTIP_H
#define SOLTOOLTIP_H

#include <QObject>


class QAbstractButton;
class SolToolTipBallon;

/**
 * 커스텀 툴팁을 표시하기 위한 이벤트 필터 객체입니다.
 *
 * @see SolTooltipFilter::setBubbleToolTip(widget, "tool tip");
 */
class SolTooltipFilter : public QObject
{
    Q_OBJECT

    explicit SolTooltipFilter(QObject* parent = nullptr);

public:
    /**
     * 버블 툴팁을 추가합니다.
     * 
     * @param inTargetWidget 툴팁을 추가할 위젯 
     * @param inToolTip 툴팁 내용
     */
    static void setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip);

    static void setCheckableButtonToolTip(QAbstractButton* inTargetWidget, const QString& inOnCheckToolTip, const QString& inOffCheckToolTip);


    virtual bool eventFilter(QObject* obj, QEvent* event) override;
};


#endif //SOLTOOLTIP_H
