// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_SOLGUARD_H
#define SOLTRANSLATOR_SOLGUARD_H

#include <QPainter>
#include <qtclasshelpermacros.h>

#include <functional>


class SolGuard {
};

/**
 * 범위를 벗어날 때, 지정된 동작을 수행하는 간단한 RAII 가드 클래스.
 * 스코프를 벗어나 객체가 소멸될 때, endFunctor가 호출됩니다. 
 */
class SolGeneralGuard
{
protected:
    explicit SolGeneralGuard() = default;

public:
    explicit SolGeneralGuard(std::function<void()>&& inEndFunctor)
    : _endFunctor(inEndFunctor) {};

    ~SolGeneralGuard() { _endFunctor(); } ;

private:
    std::function<void()> _endFunctor;

    Q_DISABLE_COPY_MOVE(SolGeneralGuard)
};

/**
 * 균형 잡힌 함수 호출을 위한 
 * RAII기반 가드 클래스입니다.
 * (like new / delete)
 * 생성자 템플릿 특수화를 통해 사용할 수 있습니다.
 */
template <typename T>
class SolTemplateGuard: public SolGeneralGuard
{
public:
    /** QPainter 특수화 */
    explicit SolTemplateGuard(QPainter* inIns) requires std::same_as<T, QPainter>
        : SolGeneralGuard([inIns, prvPen = inIns->pen()]()
        {
            inIns->setPen(prvPen);
        }) {}

private:
    std::function<void()> _startFunctor;

    Q_DISABLE_COPY_MOVE(SolTemplateGuard)
};


#endif //SOLTRANSLATOR_SOLGUARD_H