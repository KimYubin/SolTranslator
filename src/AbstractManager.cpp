//
// Created by YubinKim on 25/03/19 수.
//

#include "AbstractManager.h"

#include "FinTranslatorCore.h"

AbstractManager::AbstractManager(FinTranslatorCore* parent) : QObject(parent)
{
}

FinTranslatorCore* AbstractManager::getFinCore() const
{
    if (FinTranslatorCore* Fin = qobject_cast<FinTranslatorCore*>(parent()))
    {
        return Fin;
    }
    else
    {
        qWarning("AbstractManager::getFinTranslatorCore()");
        return nullptr;
    }
}
