// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINUTILIBRARY_H
#define FINUTILIBRARY_H


class QObject;
class QWidget;
class QApplication;

namespace Fin
{
/**
 * Other QObject와 그 부모가 this인지 재귀적으로 확인합니다.
 * @param inThis 비교 주체
 * @param inOther 비교 대상. 부모 객체도 확인합니다.
 * @return 
 */
bool isThis(const QObject* inThis, const QObject* inOther);


/** 앱 전체 기본 QFont 셋팅을 유지하면서 PreferNoHinting으로 설정합니다. */
void noHintingFont();

}



#endif //FINUTILIBRARY_H
