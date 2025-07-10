//
// Created by YubinKim on 25/07/10 목.
//

#include "FinUtilibrary.h"

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
