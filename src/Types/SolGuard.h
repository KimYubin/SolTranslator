// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLGUARD_H
#define SOLTRANSLATOR_SOLGUARD_H

#include "SolTypes.h"

namespace Sol
{
/**
 * The SolGeneralGuard class is a RAII-style guard class.
 * It executes registered callback when resource is destroyed.
 */
class SolGeneralGuard
{
public:
    [[nodiscard]]
    explicit SolGeneralGuard(Callback<void(void)>&& inEndFunctor)
        : _endFunctor(std::move(inEndFunctor))
    {}

    ~SolGeneralGuard()
    {
        if (_endFunctor)
        {
            _endFunctor();
        }
    }

private:
    Callback<void(void)> _endFunctor;

    Q_DISABLE_COPY_MOVE(SolGeneralGuard)
};
} // namespace Sol

#endif //SOLTRANSLATOR_SOLGUARD_H
