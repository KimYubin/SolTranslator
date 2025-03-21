//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TLENGINE_H
#define TLENGINE_H
#include <QObject>

#include "FinTypes.h"


class TranslateManager;
class TranslateUnit;

class TlUnitFactory : public QObject
{
    Q_OBJECT

private:
    explicit TlUnitFactory(QObject* parent = nullptr);

public:
    static TlUnitFactory& get()
    {
        static TlUnitFactory* instance = new TlUnitFactory();
        return *instance;
    }

    TranslateUnit* NewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                                  , TranslateManager* translateManager);
};


#endif //TLENGINE_H
