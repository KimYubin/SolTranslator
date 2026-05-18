// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SETTINGWIDGETFACTORY_H
#define SOLTRANSLATOR_SETTINGWIDGETFACTORY_H

#include "Types/OptionData.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

#include <QObject>

class OptionGroupBox;
class SolButton;
class QLineEdit;
class QGridLayout;
class QVBoxLayout;
class QGroupBox;
class SettingCard;

class CardFactory : public QObject
{
    Q_OBJECT

public:
    static Expected<SettingCard*> createStringSaver(QWidget* inParent
                                                  , const OptionData& inOptData
                                                  , const QString& inCurrentVal
                                                  , Callback<void(const QString&)>&& inSetFunction);

    static Expected<SettingCard*> createDoubleSpin(QWidget* inParent
                                                 , const OptionData& inOptData
                                                 , const double inCurrentVal
                                                 , Callback<void(const double)>&& inSetFunction);
};


class OptionWidgetFactory : public QObject
{
    Q_OBJECT

public:
    /**
     * 새로운 GroupBox와 GroupBox 내부에서 사용되는 VBoxLayout을 만들어 제공합니다.
     * inParentLayout의 위치에 추가되는 동작을 포함합니다.
     * 
     * @param inGroupTitle GroupBox 타이틀에 사용할 이름 
     * @param inParentLayout GroupBox가 배치될 상위 레이아웃
     * @param inRow inParentLayout row
     * @param inColumn inParentLayout row
     * @param inAlignment inParentLayout에서 Alignment
     * @return 그룹박스와 그룹박스 내부에 배치된 VBoxLayout을 반환합니다.
     */
    static OptionGroupBox* createOptionGroupBox(const QString& inGroupTitle
                                              , QGridLayout* inParentLayout
                                              , const int inRow
                                              , const int inColumn
                                              , const Qt::Alignment inAlignment = Qt::Alignment());

    static OptionGroupBox* createOptionGroupBox(const QString& inGroupTitle
                                              , QGridLayout* inParentLayout
                                              , const int inRow
                                              , const int inColumn
                                              , const int inRowSpan
                                              , const int inColumnSpan
                                              , const Qt::Alignment inAlignment = Qt::Alignment());
};


#endif //SOLTRANSLATOR_SETTINGWIDGETFACTORY_H
