//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TRANSLATEUNITGOOGLE_H
#define TRANSLATEUNITGOOGLE_H

#include "TranslateUnit.h"


class TranslateUnitGoogle final : public TranslateUnit
{
    Q_OBJECT

public:
    TranslateUnitGoogle(TranslateManager* parent);

protected:
    virtual void requestTranslate() override;
    virtual void replyTranslate(QNetworkReply* reply) override;

};



#endif //TRANSLATEUNITGOOGLE_H
