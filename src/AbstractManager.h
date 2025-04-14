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
    explicit AbstractManager(FinTranslatorCore* parent);

};


#endif //ABSTRACTMANAGER_H
