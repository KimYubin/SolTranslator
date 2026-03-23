// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLGUARD_H
#define SOLTRANSLATOR_SOLGUARD_H

#include <qtclasshelpermacros.h>

#include <functional>

class QPainter;

/**
 * RAII 스타일 가드 클래스입니다.
 * endFunctor만 등록하면, 범위를 벗어날 때 함수를 호출할 수 있습니다. 
 * 두 함수를 등록하면, 균형 잡힌 함수 호출(like new / delete)을 할 수 있습니다.
 */
class SolGeneralGuard
{
public:
    [[nodiscard]]
    explicit SolGeneralGuard(std::move_only_function<void(void)>&& inStartFunctor
                           , std::move_only_function<void(void)>&& inEndFunctor)
        : _startFunctor(std::move(inStartFunctor))
        , _endFunctor(std::move(inEndFunctor))
    {
        _startFunctor();
    };

    /**
     * 
     */
    [[nodiscard]]
    explicit SolGeneralGuard(std::move_only_function<void(void)>&& inEndFunctor)
        : _endFunctor(std::move(inEndFunctor))
    {};

    ~SolGeneralGuard()
    {
        _endFunctor();
    };

private:
    std::move_only_function<void(void)> _startFunctor;
    std::move_only_function<void(void)> _endFunctor;

    Q_DISABLE_COPY_MOVE(SolGeneralGuard)
};


/**
 * Painter의 pen을 rollback하기 위한 RAII 스타일 가드 클래스입니다.
 * @see 
 */
class PainterPenStateGuard : public SolGeneralGuard
{
public:
    [[nodiscard]]
    explicit PainterPenStateGuard(QPainter* inPainter);
};


/**
 * Painter의 font를 rollback하기 위한 RAII 스타일 가드 클래스입니다.
 * @see 
 */
class PainterFontStateGuard : public SolGeneralGuard
{
public:
    [[nodiscard]]
    explicit PainterFontStateGuard(QPainter* inPainter);
};


#endif //SOLTRANSLATOR_SOLGUARD_H
