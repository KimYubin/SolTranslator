// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef LAYOUTTEXTEDIT_H
#define LAYOUTTEXTEDIT_H
#include "CustomMenuTextEdit.h"


class QHBoxLayout;

/**
 * 레이아웃이 포함된 텍스트 에디터입니다.
 * 오버레이 버튼 등을 추가할 수 있습니다.
 */
class LayoutTextEdit : public MenuTextEdit
{
    Q_OBJECT

public:
    LayoutTextEdit(QWidget* parent = nullptr);
    ~LayoutTextEdit() override;

    virtual void resizeEvent(QResizeEvent* event) override;

    QHBoxLayout* getLayout() const { return _layout; };

protected:
    QWidget* _bottomWidget;
    QHBoxLayout* _layout;
};


#endif //LAYOUTTEXTEDIT_H
