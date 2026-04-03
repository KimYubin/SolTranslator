// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLWIDGETFACTORY_H
#define SOLTRANSLATOR_SOLWIDGETFACTORY_H

#include <QObject>

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
                                 , const Action inAction
                                 , std::move_only_function<void()>&& inFunc);

    /**
     * Create a common usable copy button.
     * 
     * @param inParent The parent widget of the copy button. Also used for the position of the copy completion pop-up.
     * @param inCopyStringFunc A functor that provides the string to copy.
     * @return 
     */
    static SolButton* createCopyButton(QWidget* inParent
                                     , std::move_only_function<QString(void)>&& inCopyStringFunc);

    static SolButton* createToggleButton(QWidget* inParent
                                       , std::move_only_function<void(void)>&& inToggleFunc);

    static SolButton* createReTranslateButton(QWidget* inParent
                                            , std::move_only_function<void(void)>&& inTranslateFunc);
    static SolButton* createViewInPopup(QWidget* inParent
                                      , std::move_only_function<void(void)>&& inFunc);
    static SolButton* createDeleteTranslation(QWidget* inParent
                                            , std::move_only_function<void(void)>&& inFunc);
};


#endif //SOLTRANSLATOR_SOLWIDGETFACTORY_H
