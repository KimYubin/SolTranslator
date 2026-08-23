// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H
#include <QComboBox>
#include <QPointer>

namespace Sol
{
enum class Action;
/**
 * The DropdownMenu class is a custom QComboBox.
 * Ignore wheel events when it does not have tab focus.
 * Reset the view mouse hover selection state when reopening the menu.
 */
class DropdownMenu : public QComboBox
{
    Q_OBJECT

public:
    explicit DropdownMenu(QWidget* inParent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void showPopup() override;
    void hidePopup() override;

    void setShortcut(const QKeySequence& inKey);

    void setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey);
    void setToolTipAction(const QString& inToolTip, const Action inAction);

    void changeShortcut(const QKeySequence& inKey);
    void setAction(const Action inAction);


protected:
    void wheelEvent(QWheelEvent* inEvent) override;

private:
    /** Reset the view mouse hover selection state when reopening the menu. */
    void resetViewCurrentIndex();

    QPointer<QShortcut> _shortcut;
};


} // namespace Sol

#endif //DROPDOWNMENU_H
