// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_SOLGUARD_H
#define SOLTRANSLATOR_SOLGUARD_H

#include <QPainter>
#include <qtclasshelpermacros.h>

#include <functional>

/**
 * RAII 스타일 가드 클래스입니다.
 * 두 함수를 등록해 균형 잡힌 함수 호출(like new / delete)을 하거나,
 * endFunctor만 등록해, 범위를 벗어날 때, 지정된 동작을 수행하도록 설정할 수 있습니다.
 * 
 */
class SolGeneralGuard
{
public:
    explicit SolGeneralGuard(std::function<void()>&& inStartFunctor
                           , std::function<void()>&& inEndFunctor)
        : _startFunctor(inStartFunctor), _endFunctor(inEndFunctor)
    {
        _startFunctor();
    };

    /**
     * 
     */
    explicit SolGeneralGuard(std::function<void()>&& inEndFunctor)
        : _endFunctor(inEndFunctor) {};

    ~SolGeneralGuard()
    {
        _endFunctor();
    };

private:
    std::function<void()> _startFunctor;
    std::function<void()> _endFunctor;

    Q_DISABLE_COPY_MOVE(SolGeneralGuard)
};


/**
 * Painter의 pen을 rollback하기 위한 RAII 스타일 가드 클래스입니다.
 */
class PainterPenStateGuard : public SolGeneralGuard
{
public:
    explicit PainterPenStateGuard(QPainter* inPainter)
        : SolGeneralGuard([inPainter, prvPen = inPainter->pen()]()
        {
            inPainter->setPen(prvPen);
        }) {}
};


#endif //SOLTRANSLATOR_SOLGUARD_H
