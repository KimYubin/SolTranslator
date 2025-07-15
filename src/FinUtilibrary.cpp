// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinUtilibrary.h"

#include <QApplication>
#include <QWidget>
#include <qfont.h>


bool Fin::isThis(const QObject* inThis, const QObject* inOther)
{
    bool bIsOtherThis = false;

    const QObject* parentObj = inOther;
    while (parentObj != nullptr)
    {
        if (parentObj == inThis)
        {
            bIsOtherThis = true;
            break;
        }
        parentObj = parentObj->parent();
    }
    return bIsOtherThis;
}
namespace Fin::Internal
{

template <typename T>
concept HasFontFunctions = requires(T* t)
{
    { t->font() } -> std::convertible_to<QFont>;
    { t->setFont(std::declval<QFont>()) };
};

template <HasFontFunctions T>
void noHintingFont(T* inOutWidget)
{
    QFont qfont = inOutWidget->font();
    qfont.setHintingPreference(QFont::PreferNoHinting);
    qfont.setStyleStrategy(QFont::PreferAntialias);
    inOutWidget->setFont(qfont);
}

}

void Fin::noHintingFont(QWidget* inOutWidget)
{
    Fin::Internal::noHintingFont(inOutWidget);
}

void Fin::noHintingFont(QApplication* inOutWidget)
{
    Fin::Internal::noHintingFont(inOutWidget);
}
