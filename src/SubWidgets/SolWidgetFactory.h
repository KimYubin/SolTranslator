// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLWIDGETFACTORY_H
#define SOLTRANSLATOR_SOLWIDGETFACTORY_H

#include "Types/SolTypes.h"

#include <QObject>

namespace Sol
{
enum class Action;
class SolButton;

class SolWidgetFactory : public QObject
{
    Q_OBJECT

public:
    static SolButton* createButton(QWidget* inParent
                                 , const QIcon& inIcon
                                 , const Qt::FocusPolicy inPolicy
                                 , const QString& inToolTip
                                 , const Action inAction);

    /**
     * Create a common usable copy button.
     * 
     * @param inParent The parent widget of the copy button. Also used for the position of the copy completion pop-up.
     * @param inCopyStringFunc A functor that provides the string to copy.
     * @return 
     */
    static SolButton* createCopyButton(QWidget* inParent
                                     , Callback<QString()>&& inCopyStringFunc);

    static SolButton* createToggleButton(QWidget* inParent
                                       , Callback<void()>&& inToggleFunc);

    static SolButton* createReTranslateButton(QWidget* inParent
                                            , Callback<void()>&& inTranslateFunc);
    static SolButton* createViewInPopup(QWidget* inParent
                                      , Callback<void()>&& inFunc);
    static SolButton* createDeleteTranslation(QWidget* inParent
                                            , Callback<void()>&& inFunc);
};


} // namespace Sol

#endif //SOLTRANSLATOR_SOLWIDGETFACTORY_H
