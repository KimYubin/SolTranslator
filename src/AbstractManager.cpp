//
// Created by YubinKim on 25/03/19 수.
//

#include "AbstractManager.h"

AbstractManager::AbstractManager(FinTranslator* parent): QObject(parent)
{
}

FinTranslator* AbstractManager::getFinTranslator() const
{
    if (FinTranslator* Fin = dynamic_cast<FinTranslator*>(parent()))
    {
        return Fin;
    }
    else
    {
        qWarning("AbstractManager::getFinTranslator()");
        return nullptr;
    }
}
