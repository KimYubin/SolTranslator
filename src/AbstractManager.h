//
// Created by YubinKim on 25/03/19 수.
//

#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H
#include <QObject>


class FinTranslatorCore;

class AbstractManager : public QObject
{
    Q_OBJECT

public:
    AbstractManager(FinTranslatorCore* parent);

protected:
    FinTranslatorCore* getFinCore() const;
};


#endif //ABSTRACTMANAGER_H
