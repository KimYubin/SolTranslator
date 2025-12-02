// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SolTypes.h"

#include <QDir>

#include <qstandardpaths.h>

#include "SolUtilibrary.h"
#include "Widgets/ITranslateWidget.h"

#include "../external/magic_enum.hpp"

namespace sol::Internal::DirName
{
const QString SAVE = "save";
const QString HISTORY = "history";
}
namespace sol::Internal::FileName
{
const QString API_KEY = "api";
const QString CONFIG_FILE = "SolConfig.ini";
const QString TRANSLATE_HISTORY = "Translate_History.json";
const QString HISTORY_DB = "Sol_Translation_History.sqlite";
}


const std::unordered_map<LangType, LangInfo> Langs::langs =
{
    {LangType::NONE, {LangType::NONE, sol::enumToQStr(LangType::NONE), u8"NONE", u8"NONE"}}
  , {LangType::AUTO, {LangType::AUTO, u8"Auto", u8"Auto", u8"Auto"}}
  , {LangType::en, {LangType::en, u8"en", u8"English", u8"English"}}
  , {LangType::ko, {LangType::ko, u8"ko", u8"Korean", u8"한국어"}}
  , {LangType::ja, {LangType::ja, u8"ja", u8"Japanese", u8"日本語"}}
};

QString EngineName::getName(EngineType inEngineType)
{
    switch (inEngineType)
    {
    case EngineType::Default:
        break;
    case EngineType::Google:
        break;
    case EngineType::OpenAI:
        break;
    case EngineType::Size:
        break;
    default: ;
    }

    return sol::enumToQStr(inEngineType);
}

QString SolPaths::getSolAppPath(const QString& inSecondaryDir, const QString& inFilePath)
{
    const QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
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

QString SolPaths::getLogPath()
{
    return getSolAppPath("logs", "log.txt");
}

QString SolPaths::getConfigPath()
{
    return getSolAppPath(sol::Internal::DirName::SAVE, sol::Internal::FileName::CONFIG_FILE);
}

QString SolPaths::getApiKeyPath()
{
    return getSolAppPath(sol::Internal::DirName::SAVE, sol::Internal::FileName::API_KEY);
}

QString SolPaths::getTranslateHistoryFilePath()
{
    return getSolAppPath(sol::Internal::DirName::HISTORY, sol::Internal::FileName::TRANSLATE_HISTORY);
}

QString SolPaths::getHistoryDBFilePath()
{
    return getSolAppPath(sol::Internal::DirName::HISTORY, sol::Internal::FileName::HISTORY_DB);
}

TranslateRequestInfo::TranslateRequestInfo(ITranslateWidget* inTrTargetWidget
                                         , const EngineType inEngineType
                                         , const QString& inOriginText
                                         , const TextStyle inTextFormat
                                         , const LangType inSourceLang
                                         , const LangType inTargetLang
                                         , QObject* inCompleteContext
                                         , std::function<void(const QString&)>&& inCallbackTranslateComplete
                                         , QObject* inStreamContext
                                         , std::optional<std::function<void(const QString&)>>&& incallbackTranslateStreaming)
    : trTargetWidget(inTrTargetWidget)
    , engineType(inEngineType)
    , originText(inOriginText)
    , textFormat(inTextFormat)
    , sourceLang(inSourceLang)
    , targetLang(inTargetLang)
    , completeContext(inCompleteContext)
    , callbackTranslateComplete(inCallbackTranslateComplete)
    , streamContext(inStreamContext)
    , callbackTranslateStreaming(incallbackTranslateStreaming)
{}

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

QString Langs::GetLocaleName(const LangType inLangType)
{
    // todo: 현지화 버전으로 변경할 예정. 현재는 자국명.
    return GetLangInfo(inLangType).endonymName;
}

std::vector<LangType> Langs::GetLanguageList()
{
    std::vector<LangType> languageList;
    for (LangType lt = LangType::AUTO; lt != LangType::Size; lt = static_cast<LangType>(static_cast<int>(lt) + 1))
    {
        const QString langName = Langs::GetLocaleName(lt);
        if (langName != "NONE")
        {
            languageList.push_back(lt);
        }
    }

    return languageList;
}

bool Langs::IsContainName(const LangType inLangType, const QString& inLangName)
{
    // todo: 로케일 현지화 이름 및 각국 언어별 명칭을 모두 찾도록 만들어야합니다.
    // todo: 현지명, 영어명, 코드명, 자국명(모든언어 검색X. 자국명만 따로 모아놓아야합니다.)
    const QString LangName = Langs::GetLocaleName(inLangType);
    return LangName.contains(inLangName, Qt::CaseInsensitive);
}
