// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_ENGINSELECTER_H
#define SOLTRANSLATOR_ENGINSELECTER_H

#include "DropdownMenu.h"

/**
 * Engine selection combo box
 */
class EngineSelector : public DropdownMenu
{
    Q_OBJECT

public:
    explicit EngineSelector(QWidget* parent = nullptr);

    void setCurrentIndexChanged(std::move_only_function<void(const int)>&& inFunctor);

};

#endif //SOLTRANSLATOR_ENGINSELECTER_H