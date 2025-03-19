//
// Created by YubinKim on 25/03/19 수.
//

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include "fintranslator.h"


class AbstractManager : public QObject
{
    Q_OBJECT

public:
    AbstractManager(FinTranslator* parent);

protected:
    FinTranslator* getFinTranslator() const;
};


#endif //ABSTRACTMANAGER_H
