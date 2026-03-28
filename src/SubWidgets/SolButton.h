// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLPUSHBUTTON_H
#define SOLTRANSLATOR_SOLPUSHBUTTON_H

#include <QPushButton>


enum class Action;

class SolButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SolButton(QWidget* parent = nullptr)
        : QPushButton(parent)
    {}

    SolButton(const QString& text, QWidget* parent)
        : QPushButton(text, parent)
    {}

    SolButton(const QIcon& icon, const QString& text, QWidget* parent)
        : QPushButton(icon, text, parent)
    {}

    void setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey);
    void setToolTipAction(const QString& inToolTip, const Action inAction/* = Action::None*/);

    void setCheckIcon(const QString& inOnFileName, const QString& inOffFileName);

    void setCheckToolTipShortcut(const QString& inOnToolTip
                               , const QString& inOffToolTip
                               , const QKeySequence& inKey = QKeySequence());
    void setCheckToolTipAction(const QString& inOnToolTip
                             , const QString& inOffToolTip
                             , const Action inAction/* = Action::None*/);

    /**
     * 
     */
    void changeShortcut(const QKeySequence& inKey);
};


#endif //SOLTRANSLATOR_SOLPUSHBUTTON_H
