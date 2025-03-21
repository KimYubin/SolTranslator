//
// Created by YubinKim on 25/03/17 월.
//

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "TranslateUnit.h"


class TranslateUnitOpenAI final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit TranslateUnitOpenAI(const TranslateRequestInfo& inTranslateRequestInfo
                               , TranslateManager* parent);

protected:
    virtual void requestTranslate() override;
    virtual void replyTranslateFinished(QNetworkReply* reply) override;

};



#endif //TRANSLATEUNITOPENAI_H
