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
                                     , const LangType inSourceLang
                                     , const LangType inTargetLang)
{
    if (inText.isEmpty())
    {
        updateTranslatedText(inText);
        return;
    }

    // cache 텍스트 관련 작업에서 사용되기 때문에, 먼저 업데이트
    originText = inText;
    sourceLang = inSourceLang;
    targetLang = inTargetLang;

    if (TranslateManager* translate_manager = dynamic_cast<TranslateManager*>(parent()))
    {
        auto [bIsFind, findCache] = translate_manager->findCachingText(inText, inTargetLang);
        if (bIsFind)
        {
            // 캐싱되어있다면 업데이트 합니다.
            // 내부에서 캐시의 순서를 최신으로 변경합니다.
            updateTranslatedText(findCache);
            return;
        }
    }

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

void TranslateUnit::updateTranslatedText(const QString& translatedText)
{
    if (translatedText.isEmpty() == false)
    {
        if (TranslateManager* translate_manager = qobject_cast<TranslateManager*>(parent()))
        {
            translate_manager->setCacheText(originText, translatedText, targetLang);
        }
    }

    // 빈 문자열도 적용합니다.
    emit ApplyCompletedTranslate(translatedText);
    deleteLater();
}
