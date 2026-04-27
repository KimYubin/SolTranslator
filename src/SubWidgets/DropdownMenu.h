// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H
#include <QComboBox>

/**
 * The DropdownMenu class is a custom QComboBox.
 * Ignore wheel events when it does not have tab focus.
 * Reset the view mouse hover selection state when reopening the menu.
 */
class DropdownMenu : public QComboBox
{
    Q_OBJECT

public:
    explicit DropdownMenu(QWidget* parent = nullptr);

    virtual void showPopup() override;
    virtual void hidePopup() override;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    /** Reset the view mouse hover selection state when reopening the menu. */
    void resetViewCurrentIndex();
};


#endif //DROPDOWNMENU_H
