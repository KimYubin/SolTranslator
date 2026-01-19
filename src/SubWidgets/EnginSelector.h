// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_ENGINSELECTER_H
#define SOLTRANSLATOR_ENGINSELECTER_H
#include "DropdownMenu.h"


/**
 * 엔진 선택 콤보박스
 */
class EnginSelector : public DropdownMenu
{
    Q_OBJECT

public:
    explicit EnginSelector(QWidget* parent = nullptr);
    
    void setCurrentIndexChanged(std::function<void(const int)>&& inFunctor);

};

#endif //SOLTRANSLATOR_ENGINSELECTER_H