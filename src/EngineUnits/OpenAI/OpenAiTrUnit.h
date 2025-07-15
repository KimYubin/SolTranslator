// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEUNITOPENAI_H
#define TRANSLATEUNITOPENAI_H

#include "EngineUnits/TranslateUnit.h"


class OpenAiTrUnit final : public TranslateUnit
{
    Q_OBJECT

public:
    explicit OpenAiTrUnit(const TranslateRequestInfo& inTranslateRequestInfo
                        , TranslateManager* parent);

protected:
    virtual void requestTranslate() override;

    void chatTranslate(const bool bIsStreaming);

    void onReadyRead(QNetworkReply* reply);

    virtual void replyTranslateFinished(QNetworkReply* reply) override;
};


#endif //TRANSLATEUNITOPENAI_H
