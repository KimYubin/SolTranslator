//
// Created by YubinKim on 25/06/22 일.
//

#ifndef FINPOINT_H
#define FINPOINT_H
#include "EngineUnits/TranslateUnit.h"

class FinPointTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit FinPointTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                          , TranslateManager* parent);

protected:
    virtual void requestTranslate() override;

    void chatTranslate(const bool bIsStreaming);

    void onReadyRead(QNetworkReply* reply);

    virtual void replyTranslateFinished(QNetworkReply* reply) override;

public:
    void setDebugMode(const bool inDebugMode) { bDebugMode = inDebugMode; };

private:
    bool bDebugMode = false;
};


#endif //FINPOINT_H
