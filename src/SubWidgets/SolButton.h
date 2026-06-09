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

    explicit SolButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {}

    SolButton(const QIcon& icon, const QString& text, QWidget* parent)
        : QPushButton(icon, text, parent)
    {}

    void setCheckIcon(const QString& inOnFileName, const QString& inOffFileName);

    void setToolTipShortcut(const QString& inToolTip, const QKeySequence& inKey);
    void setToolTipAction(const QString& inToolTip, const Action inAction);


    void setCheckToolTipShortcut(const QString& inOnToolTip
                               , const QString& inOffToolTip
                               , const QKeySequence& inKey = QKeySequence());
    void setCheckToolTipAction(const QString& inOnToolTip
                             , const QString& inOffToolTip
                             , const Action inAction);

    void changeShortcut(const QKeySequence& inKey);
    void setAction(const Action inAction);

private:

};


#endif //SOLTRANSLATOR_SOLPUSHBUTTON_H
