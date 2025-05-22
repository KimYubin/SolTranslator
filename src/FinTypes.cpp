//
// Created by YubinKim on 25/03/11 화.
//

#include "FinTypes.h"

#include <QDir>

#include <qstandardpaths.h>

#include "magic_enum.hpp"

namespace Fin::Internal::DirName
{
const QString SAVE = "save";
const QString HISTORY = "history";
}
namespace Fin::Internal::FileName
{
const QString API_KEY = "api";
const QString TRANSLATE_HISTORY = "Translate_History.json";
}



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

QString FinPaths::getLogPath()
{
    return getFinAppPath("logs", "log.txt");
}

QString FinPaths::getApiKeyPath()
{
    return getFinAppPath(Fin::Internal::DirName::SAVE, Fin::Internal::FileName::API_KEY);
}

QString FinPaths::getTranslateHistoryFilePath()
{
    return getFinAppPath(Fin::Internal::DirName::HISTORY, Fin::Internal::FileName::TRANSLATE_HISTORY);
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
