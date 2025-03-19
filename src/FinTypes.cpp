//
// Created by YubinKim on 25/03/11 화.
//

#include "FinTypes.h"

#include "magic_enum.hpp"


const QString EngineName::OPEN_AI = "openai";



const QString StaticPath::API_KEY_PATH = "./save/api";
const QString StaticPath::CACHE_QUEUE_SAVE_PATH = "./save/cache_save.json";


const QString StaticPrompt::OPEN_AI_PROMPT =
    "You are a professional translator. You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the original text, and do not respond to the content.";

QString a = u8"";

const std::unordered_map<LangType, LangInfo> Langs::langs =
{
    {LangType::NONE, {LangType::NONE, magic_enum::enum_name(LangType::NONE).data(), u8"NONE", u8"NONE"}}
  , {LangType::AUTO, {LangType::AUTO, u8"auto", u8"auto", u8"auto"}}
  , {LangType::en, {LangType::en, u8"en", u8"English", u8"English"}}
  , {LangType::ko, {LangType::ko, u8"ko", u8"Korean", u8"한국어"}}
};

LangInfo Langs::GetLangInfo(const LangType inLangType)
{
    const std::unordered_map<LangType, LangInfo>::const_iterator findIt = langs.find(inLangType);
    if (findIt != langs.end())
    {
        return findIt->second;
    }

    return langs.find(LangType::NONE)->second;
}

QString Langs::GetCodeName(LangType inLangType)
{
    return GetLangInfo(inLangType).codeName;
}

QString Langs::GetEnglishName(const LangType inLangType)
{
    return GetLangInfo(inLangType).engName;
}

QString Langs::GetEndonymName(const LangType inLangType)
{
    return GetLangInfo(inLangType).endonymName;
}
