// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLWIDGETFACTORY_H
#define SOLTRANSLATOR_SOLWIDGETFACTORY_H
#include <QObject>
#include <QString>

#include <functional>


class SolButton;

class SolWidgetFactory : public QObject
{
    Q_OBJECT

public:
    /**
     * Create a common usable copy button.
     * 
     * @param inParent The parent widget of the copy button. Also used for the position of the copy completion pop-up.
     * @param inCopyStringFunc A functor that provides the string to copy.
     * @return 
     */
    static SolButton* createCopyButton(QWidget* inParent, std::move_only_function<QString(void)>&& inCopyStringFunc);
};


#endif //SOLTRANSLATOR_SOLWIDGETFACTORY_H
