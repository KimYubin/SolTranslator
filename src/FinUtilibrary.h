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
}



#endif //FINUTILIBRARY_H
