//
// Created by YubinKim on 25/06/25 수.
//

#include "DropdownMenu.h"

#include <qevent.h>

DropdownMenu::DropdownMenu(QWidget* parent): QComboBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void DropdownMenu::wheelEvent(QWheelEvent* event)
{
    if (this->hasFocus())
    {
        QComboBox::wheelEvent(event);
    }
    else
    {
        // 포커스 없으면 wheel 이벤트 무시
        event->ignore();
    }
}
