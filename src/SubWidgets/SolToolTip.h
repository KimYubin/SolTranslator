// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTOOLTIP_H
#define SOLTOOLTIP_H

#include <QObject>

struct ToolTipData;
enum class Action;
class QAbstractButton;

/**
 * The SolToolTip class is a helper class for displaying custom tooltips.
 */
class SolToolTip : public QObject
{
    Q_OBJECT

    explicit SolToolTip(QObject* parent = nullptr);

public:
    static bool isValidToolTip(const QWidget* inWidget);
    static ToolTipData getToolTipData(const QWidget* inWidget);

    static void setToolTipProperty(QWidget* inWidget, ToolTipData inToolTipData);

    /**
     * 버블 툴팁을 추가합니다.
     *
     * @param inWidget 툴팁을 추가할 위젯
     * @param inToolTip 툴팁 내용
     */
    static void setToolTip(QWidget* inWidget, const QString& inToolTip);


    static void setToolTipShortcut(QWidget* inWidget
                                 , const QString& inToolTip
                                 , const QKeySequence& inKey);

    static void setToolTipAction(QWidget* inWidget
                               , const QString& inToolTip
                               , const Action inAction);

    static void changeShortcut(QWidget* inWidget
                             , const QKeySequence& inKey);

    static void setAction(QWidget* inWidget
                        , const Action inAction);


    static void setCheckButtonToolTip(QAbstractButton* inButton
                                    , const QString& inOnToolTip
                                    , const QString& inOffToolTip
                                    , const QKeySequence& inKey);
};

#endif //SOLTOOLTIP_H
