//
// Created by YubinKim on 25/07/10 목.
//

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

void Fin::noHintingFont(QWidget* inOutWidget)
{
    QFont qfont = inOutWidget->font();
    // qfont.setHintingPreference(QFont::PreferNoHinting);
    // qfont.setStyleStrategy(QFont::PreferAntialias);
    inOutWidget->setFont(qfont);
}

void Fin::noHintingFont(QApplication* inOutWidget)
{
    QFont qfont = inOutWidget->font();
    // qfont.setHintingPreference(QFont::PreferNoHinting);
    // qfont.setStyleStrategy(QFont::PreferAntialias);
    inOutWidget->setFont(qfont);
}
