//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TRANSLATEUNITGOOGLE_H
#define TRANSLATEUNITGOOGLE_H

#include "EngineUnits/TranslateUnit.h"


class TranslateUnitGoogle final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit TranslateUnitGoogle(const TranslateRequestInfo& inTranslateRequestInfo
                               , TranslateManager* parent);

protected:
    virtual void requestTranslate() override;
    virtual void replyTranslateFinished(QNetworkReply* reply) override;

};



#endif //TRANSLATEUNITGOOGLE_H
