// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSHAREDGUARD_H
#define SOLTRANSLATOR_SOLSHAREDGUARD_H
#include "SolTypes.h"

#include <cstddef>

class SolGeneralGuard;

template <typename T>
class RefCount
{
public:
    [[nodiscard]]
    explicit RefCount(T* inRefPtr) noexcept
        : _refPtr(inRefPtr)
    {}

    void increment() noexcept { _counter += 1; }

    void decrement() noexcept
    {
        _counter -= 1;
        if (_counter == 0)
        {
            destroy();
            delete this;
        }
    }

    void destroy() noexcept
    {
        if (_refPtr)
        {
            delete _refPtr;
            _refPtr = nullptr;
        }
    };

private:
    std::size_t _counter = 1;
    T* _refPtr{nullptr};
};

/**
 * The SolSharedGuard class is a shareable RAII-style guard class.
 * It executes registered callbacks when all shared resources are destroyed.
 */

class SolSharedGuard
{
public:
    constexpr SolSharedGuard() noexcept = default;
    constexpr SolSharedGuard(std::nullptr_t) noexcept {} // construct empty

    [[nodiscard]]
    explicit SolSharedGuard(Callback<void(void)>&& inEndFunctor);

    [[nodiscard]] SolSharedGuard(const SolSharedGuard& inOther);
    [[nodiscard]] SolSharedGuard(SolSharedGuard&& inOther) noexcept;

    SolSharedGuard& operator=(const SolSharedGuard& inOther);
    SolSharedGuard& operator=(SolSharedGuard&& inOther) noexcept;

    ~SolSharedGuard();

    void swap(SolSharedGuard& inOther) noexcept;

private:
    void incrementRef() const noexcept;
    void decrementRef() const noexcept;

    SolGeneralGuard* _guardPtr{nullptr};
    RefCount<SolGeneralGuard>* _refCount{nullptr};
};


#endif //SOLTRANSLATOR_SOLSHAREDGUARD_H
