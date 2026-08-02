// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_UNIQUEQOBJECTPTR_H
#define SOLTRANSLATOR_UNIQUEQOBJECTPTR_H
#include <QObject>

#include <memory>


/**
 * QObject unique pointer using deleteLater()
 */
struct QObjectDeleter
{
    void operator()(QObject* inObj) const noexcept
    {
        if (inObj)
            inObj->deleteLater();
    }
};

template <std::derived_from<QObject> T>
using unique_qobject = std::unique_ptr<T, QObjectDeleter>;

template <typename T, typename... Args>
auto make_unique_qobject(Args&&... args)
{
    return unique_qobject<T>(new T(std::forward<Args>(args)...));
}


#endif //SOLTRANSLATOR_UNIQUEQOBJECTPTR_H
