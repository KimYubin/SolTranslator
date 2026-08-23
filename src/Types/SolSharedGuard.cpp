// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolSharedGuard.h"

#include "SolGuard.h"
#include "SolTypes.h"


namespace Sol
{
SolSharedGuard::SolSharedGuard(Callback<void(void)>&& inEndFunctor)
{
    _guardPtr = new SolGeneralGuard(std::move(inEndFunctor));
    _refCount = new RefCount<SolGeneralGuard>(_guardPtr);
}

SolSharedGuard::SolSharedGuard(const SolSharedGuard& inOther)
{
    inOther.incrementRef();

    _guardPtr = inOther._guardPtr;
    _refCount = inOther._refCount;
}

SolSharedGuard::SolSharedGuard(SolSharedGuard&& inOther) noexcept
{
    _guardPtr = inOther._guardPtr;
    _refCount = inOther._refCount;

    inOther._guardPtr = nullptr;
    inOther._refCount = nullptr;
}

SolSharedGuard& SolSharedGuard::operator=(const SolSharedGuard& inOther)
{
    SolSharedGuard{inOther}.swap(*this);

    return *this;
}

SolSharedGuard& SolSharedGuard::operator=(SolSharedGuard&& inOther) noexcept
{
    SolSharedGuard{std::move(inOther)}.swap(*this);

    return *this;
}

SolSharedGuard::~SolSharedGuard()
{
    decrementRef();
}

void SolSharedGuard::swap(SolSharedGuard& inOther) noexcept
{
    std::swap(_guardPtr, inOther._guardPtr);
    std::swap(_refCount, inOther._refCount);
}

void SolSharedGuard::incrementRef() const noexcept
{
    if (_refCount)
    {
        _refCount->increment();
    }
}

void SolSharedGuard::decrementRef() const noexcept
{
    if (_refCount)
    {
        _refCount->decrement();
    }
}
} // namespace Sol
