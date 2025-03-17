//
// Created by YubinKim on 25/03/10 월.
//

#include "TranslateUnit.h"

#include <iostream>

#include "ConfigManager.h"
#include "FinTypes.h"
#include "TranslateManager.h"

TranslateUnit::TranslateUnit(TranslateManager* parent) : QNetworkAccessManager(parent)
{
    connect(this, &QNetworkAccessManager::finished, this, &TranslateUnit::onReplyFinished);
}

void TranslateUnit::translateText_Impl(const QString& inText
                                     , const QString& inSourceLang
                                     , const QString& inTargetLang)
{
    if (inText.isEmpty())
    {
        emit CompletedTranslate(inText);
        deleteLater();
        return;
    }
    if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(inText, inTargetLang);
        if (bIsFind)
        {
            emit CompletedTranslate(findCache);
            deleteLater();
            return;
        }
    }

    originText = inText;
    sourceLang = inSourceLang;
    targetLang = inTargetLang;

    // to subclass
    requestTranslate();

}

void TranslateUnit::onReplyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // to subclass
        replyTranslate(reply);
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();

    deleteLater();
}
