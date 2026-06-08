// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTOOLTIP_H
#define SOLTOOLTIP_H

#include <QObject>

class QAbstractButton;

/**
 * 커스텀 툴팁을 표시하기 위한 이벤트 필터 객체입니다.
 *
 * @see SolToolTip::setBubbleToolTip
 */
class SolToolTip : public QObject
{
    Q_OBJECT

    explicit SolToolTip(QObject* parent = nullptr);

public:
    /**
     * 버블 툴팁을 추가합니다.
     *
     * @param inTargetWidget 툴팁을 추가할 위젯
     * @param inToolTip 툴팁 내용
     */
    static void setBubbleToolTip(QWidget* inTargetWidget, const QString& inToolTip);

    static void setCheckableButtonToolTip(QAbstractButton* inTargetWidget, const QString& inOnCheckToolTip, const QString& inOffCheckToolTip);
};

#endif //SOLTOOLTIP_H
