// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLPUSHBUTTON_H
#define SOLTRANSLATOR_SOLPUSHBUTTON_H

#include <QPushButton>


namespace Sol
{
enum class Action;

class SolButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SolButton(QWidget* inParent = nullptr)
        : QPushButton(inParent)
    {}

    explicit SolButton(const QString& inText, QWidget* inParent = nullptr)
        : QPushButton(inText, inParent)
    {}

    SolButton(const QIcon& inIcon, const QString& inText, QWidget* inParent)
        : QPushButton(inIcon, inText, inParent)
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

};


} // namespace Sol

#endif //SOLTRANSLATOR_SOLPUSHBUTTON_H
