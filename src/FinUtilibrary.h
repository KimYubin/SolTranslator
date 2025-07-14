//
// Created by YubinKim on 25/07/10 목.
//

#ifndef FINUTILIBRARY_H
#define FINUTILIBRARY_H
#include <QObject>


class FinUtilibrary {

};

namespace Fin
{
/**
 * Other QObject와 그 부모가 this인지 재귀적으로 확인합니다.
 * @param inThis 비교 주체
 * @param inOther 비교 대상. 부모 객체도 확인합니다.
 * @return 
 */
bool isThis(const QObject* inThis, const QObject* inOther);

/**
 * 위젯의 QFont 셋팅을 유지하면서 PreferNoHinting으로 설정합니다.
 * 
 * @param inOutWidget QFont를 변경할 위젯 
 */

void noHintingFont(QWidget* inOutWidget);
void noHintingFont(QApplication* inOutWidget);

template <typename T>
concept HasFontFunctions = requires(T* t)
{
    { t->font() } -> std::same_as<QFont>;
    { t->setFont(std::declval<QFont>()) };
};

// template <HasFontFunctions T>
// void noHintingFont(T* inOutWidget)
// {
//     noHintingFont(inOutWidget);
// }


}



#endif //FINUTILIBRARY_H
