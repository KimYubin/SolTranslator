// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H
#include <QComboBox>


/**
 * 포커스가 없을때 휠 이벤트를 무시하는 콤보박스입니다.
 */
class DropdownMenu : public QComboBox
{
    Q_OBJECT

public:
    explicit DropdownMenu(QWidget* parent = nullptr);


protected:
    virtual void wheelEvent(QWheelEvent *event) override;

private slots:


private:
};


#endif //DROPDOWNMENU_H
