// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTypes.h"

#include "SolUtilibrary.h"
#include "Utils/Tr.h"
#include "Widgets/ITranslateWidget.h"

const std::unordered_map<LangType, LangInfo> Langs::langs =
{
    {LangType::NONE, {LangType::NONE, Sol::enumToQStr(LangType::NONE), u8"NONE", u8"NONE"}}
  , {LangType::AUTO, {LangType::AUTO, u8"Auto", u8"Auto", u8"Auto"}}
  , {LangType::en, {LangType::en, u8"en", u8"English", u8"English"}}
  , {LangType::ko, {LangType::ko, u8"ko", u8"Korean", u8"한국어"}}
  , {LangType::ja, {LangType::ja, u8"ja", u8"Japanese", u8"日本語"}}
};

QString EngineHelper::displayName(const EngineType inEngineType)
{
    switch (inEngineType)
    {
    case EngineType::Google:
        return Sol::i18n(Tr::GoogleTranslate);
        break;
    case EngineType::OpenAI:
        return Sol::i18n(Tr::OpenAI);
        break;

    case EngineType::Size:
    default: ;
    }

    return Sol::enumToQStr(inEngineType);
}

EngineType EngineHelper::defaultEngineType()
{
    return EngineType::Google;
}


TranslateRequestInfo::TranslateRequestInfo(ITranslateWidget* inTrDisplayWidget
                                         , const bool inIsIgnoreCache
                                         , const EngineType inEngineType
                                         , const QString& inSourceText
                                         , const TextStyle inTextFormat
                                         , const LangType inSourceLang
                                         , const LangType inTargetLang
                                         , QObject* inCompleteContext
                                         , std::move_only_function<void(const QString&)>&& inFuncComplete
                                         , QObject* inStreamContext
                                         , std::optional<std::move_only_function<void(const QString&)>>&& inFuncStreaming)
    : trDisplayWidget(inTrDisplayWidget)
    , isIgnoreCache(inIsIgnoreCache)
    , engineType(inEngineType)
    , sourceText(inSourceText)
    , textFormat(inTextFormat)
    , sourceLang(inSourceLang)
    , targetLang(inTargetLang)
    , completeContext(inCompleteContext)
    , callbackTranslateComplete(std::move(inFuncComplete))
    , streamContext(inStreamContext)
    , callbackTranslateStreaming(std::move(inFuncStreaming))
{}

LangInfo Langs::getLangInfo(const LangType inLangType)
{
    const std::unordered_map<LangType, LangInfo>::const_iterator findIt = langs.find(inLangType);
    if (findIt != langs.end())
    {
        return findIt->second;
    }

    return langs.find(LangType::NONE)->second;
}

QString Langs::getCodeName(const LangType inLangType)
{
    return getLangInfo(inLangType).codeName;
}

QString Langs::getEnglishName(const LangType inLangType)
{
    return getLangInfo(inLangType).engName;
}

QString Langs::getEndonymName(const LangType inLangType)
{
    return getLangInfo(inLangType).endonymName;
}

QString Langs::getLocaleName(const LangType inLangType)
{
    // todo: 현지화 버전으로 변경할 예정. 현재는 자국명.
    return getLangInfo(inLangType).endonymName;
}

std::vector<LangType> Langs::getLanguageList()
{
    std::vector<LangType> languageList;
    for (LangType lt = LangType::AUTO; lt != LangType::Size; lt = static_cast<LangType>(static_cast<int>(lt) + 1))
    {
        const QString langName = getLocaleName(lt);
        if (langName != "NONE")
        {
            languageList.push_back(lt);
        }
    }

    return languageList;
}

bool Langs::containName(const LangType inLangType, const QString& inLangName)
{
    // todo: 로케일 현지화 이름 및 각국 언어별 명칭을 모두 찾도록 만들어야합니다.
    // todo: 현지명, 영어명, 코드명, 자국명(모든언어 검색X. 자국명만 따로 모아놓아야합니다.)
    const QString LangName = getLocaleName(inLangType);
    return LangName.contains(inLangName, Qt::CaseInsensitive);
}
