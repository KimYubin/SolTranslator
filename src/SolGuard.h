// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_SOLGUARD_H
#define SOLTRANSLATOR_SOLGUARD_H

#include <QPainter>
#include <qtclasshelpermacros.h>

#include <functional>

/**
 * RAII 스타일 가드 클래스입니다.
 * endFunctor만 등록하면, 범위를 벗어날 때 함수를 호출할 수 있습니다. 
 * 두 함수를 등록하면, 균형 잡힌 함수 호출(like new / delete)을 할 수 있습니다.
 */
template <typename Func>
class SolGeneralGuard
{
public:
    Q_NODISCARD_CTOR
    explicit SolGeneralGuard(std::function<Func>&& inStartFunctor
                           , std::function<Func>&& inEndFunctor)
        : _startFunctor(std::move(inStartFunctor))
        , _endFunctor(std::move(inEndFunctor))
    {
        _startFunctor();
    };

    /**
     * 
     */
    Q_NODISCARD_CTOR
    explicit SolGeneralGuard(std::function<Func>&& inEndFunctor)
        : _endFunctor(inEndFunctor)
    {};

    ~SolGeneralGuard()
    {
        _endFunctor();
    };

private:
    std::function<Func> _startFunctor;
    std::function<Func> _endFunctor;

    Q_DISABLE_COPY_MOVE(SolGeneralGuard)
};


/**
 * Painter의 pen을 rollback하기 위한 RAII 스타일 가드 클래스입니다.
 * @see 
 */
class PainterPenStateGuard : public SolGeneralGuard<void()>
{
public:
    Q_NODISCARD_CTOR
    explicit PainterPenStateGuard(QPainter* inPainter)
        : SolGeneralGuard([inPainter, prvPen = inPainter->pen()]()
        {
            inPainter->setPen(prvPen);
        })
    {}
};


#endif //SOLTRANSLATOR_SOLGUARD_H
