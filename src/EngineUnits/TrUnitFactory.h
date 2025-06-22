//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TLENGINE_H
#define TLENGINE_H
#include <QObject>

#include "FinTypes.h"


class TranslateManager;
class TranslateUnit;

class TrUnitFactory : public QObject
{
    Q_OBJECT

private:
    explicit TrUnitFactory(QObject* parent = nullptr);

public:
    static TrUnitFactory& get()
    {
        static TrUnitFactory* instance = new TrUnitFactory();
        return *instance;
    }

    TranslateUnit* NewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo
                                  , TranslateManager* translateManager);
};


#endif //TLENGINE_H
