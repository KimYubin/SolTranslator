// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H
#include <QComboBox>
#include <QPointer>

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
    explicit DropdownMenu(QWidget* parent = nullptr);

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    virtual void showPopup() override;
    virtual void hidePopup() override;

    void setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey);
    void setToolTipAction(const QString& inToolTip, const Action inAction);

    void changeShortcut(const QKeySequence& inKey);
    void setAction(const Action inAction);

    void setBubbleToolTip(const QString& inToolTip);

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    /** Reset the view mouse hover selection state when reopening the menu. */
    void resetViewCurrentIndex();

    QString _toolTip;
    QPointer<QShortcut> _shortcut;
};


#endif //DROPDOWNMENU_H
