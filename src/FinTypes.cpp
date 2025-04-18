//
// Created by YubinKim on 25/03/11 화.
//

#include "FinTypes.h"

#include <QDir>

#include <qstandardpaths.h>

#include "magic_enum.hpp"


const QString FinPaths::FinDirName::SAVE = "save";
const QString FinPaths::FinDirName::HISTORY = "history";

const QString FinPaths::FinFileName::API_KEY = "api";
const QString FinPaths::FinFileName::TRANSLATE_HISTORY = "Translate_History.json";


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

QString EngineName::getName(EngineType inEngineType)
{
    switch (inEngineType)
    {
    case EngineType::None:
        break;
    case EngineType::Google:
        break;
    case EngineType::OpenAI:
        break;
    case EngineType::Size:
        break;
    default: ;
    }
    return QString{magic_enum::enum_name(inEngineType).data()};
}

QString FinPaths::getFinAppPath(const QString& inSecondaryDir, const QString& inFilePath)
{
    const QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation	);
    if (appPath.isEmpty())
    {
        qFatal() << "Cannot determine settings storage location";
        return "";
    }

    const QDir secondaryDir{appPath + "/" + inSecondaryDir + "/"};

    if (secondaryDir.exists() == false)
    {
        if (secondaryDir.mkpath(".") == false)
        {
            qFatal() << "Invalid secondary directory path: " << inSecondaryDir;
            return "";
        }
    }

    return secondaryDir.absoluteFilePath(inFilePath);
}

QString FinPaths::getApiKeyPath()
{
    return getFinAppPath(FinDirName::SAVE, FinFileName::API_KEY);
}

QString FinPaths::getTranslateHistoryFilePath()
{
    return getFinAppPath(FinDirName::HISTORY, FinFileName::TRANSLATE_HISTORY);
}

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
