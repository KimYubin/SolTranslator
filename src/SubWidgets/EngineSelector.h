// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENGINSELECTER_H
#define SOLTRANSLATOR_ENGINSELECTER_H

#include "DropdownMenu.h"
#include "Types/SolTypes.h"

namespace Sol
{
/**
 * Engine selection combo box
 */
class EngineSelector : public DropdownMenu
{
    Q_OBJECT

public:
    explicit EngineSelector(QWidget* inParent = nullptr);

    void setCurrentIndexChanged(Callback<void(const int)>&& inFunctor);

};

} // namespace Sol

#endif //SOLTRANSLATOR_ENGINSELECTER_H