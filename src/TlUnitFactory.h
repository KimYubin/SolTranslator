//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TLENGINE_H
#define TLENGINE_H
#include <QObject>


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

    TranslateUnit* NewTranslateUnit(TranslateManager* translateManager);

};


#endif //TLENGINE_H
