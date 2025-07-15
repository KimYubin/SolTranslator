// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTYPES_H
#define FINTYPES_H
#include <QString>
#include <QObject>


enum class EngineType;
enum class LangType;

class FinTypes
{
};

struct EngineName
{
public:
    static QString getName(EngineType inEngineType);
};


/** 유저 파일 경로 */
struct FinPaths
{
private:
    static QString getFinAppPath(const QString& inSecondaryDir, const QString& inFilePath);

public:
    static QString getLogPath();
    static QString getConfigPath();
    static QString getApiKeyPath();
    static QString getTranslateHistoryFilePath();
};


/**
 * enum class 값을 베이스 타입 값으로 static casting합니다.
 * 
 * @tparam E enum class Only
 * @param e 변환 대상 enum
 * @return static_cast<uint8>(e). std::underlying_type_t<E>(e)
 */
template <typename E>
constexpr std::enable_if_t<std::is_enum_v<E>, std::underlying_type_t<E>> EnumToInt(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

namespace Fin
{
Q_NAMESPACE

enum class ScreenPopupPolicy
{
    Default

  , PrimaryScreen
  , FixedScreen
  , CursorScreen

  , Size
};
Q_ENUM_NS(ScreenPopupPolicy)


}

enum class EngineType
{
    Default = 0

  , Google
  , OpenAI
  , FinPoint
  , FinPointDebug

  , Size
};

enum class TextStyle
{
    None
  , PlainText
  , Html
  , MarkDown
  , Size
};

struct TranslateRequestInfo
{
    TranslateRequestInfo() = default;

    TranslateRequestInfo(const EngineType inEngineType
                       , const QString& inOriginText
                       , const TextStyle inTextFormat
                       , const LangType inSourceLang
                       , const LangType inTargetLang
                       , QObject* inCompleteContext
                       , std::function<void(const QString&)>&& inCallbackTranslateComplete
                       , QObject* inStreamContext = nullptr
                       , std::optional<std::function<void(const QString&)>>&& incallbackTranslateStreaming = std::nullopt)
        : engineType(inEngineType)
        , originText(inOriginText)
        , textFormat(inTextFormat)
        , sourceLang(inSourceLang)
        , targetLang(inTargetLang)
        , completeContext(inCompleteContext)
        , callbackTranslateComplete(inCallbackTranslateComplete)
        , streamContext(inStreamContext)
        , callbackTranslateStreaming(incallbackTranslateStreaming)
    {}

    EngineType engineType;
    QString originText;
    TextStyle textFormat;
    LangType sourceLang;
    LangType targetLang;
    QObject* completeContext;
    std::function<void(const QString&)> callbackTranslateComplete;
    QObject* streamContext;
    std::optional<std::function<void(const QString&)>> callbackTranslateStreaming;
};

struct LangInfo
{
public:
    LangInfo(const LangType inLangType, const QString& inCodeName, const QString& inEngName, const QString& inEndonymName)
        : langType(inLangType)
        , codeName(inCodeName)
        , engName(inEngName)
        , endonymName(inEndonymName) {}


    LangType langType;   // 언어 타입
    QString codeName;    // ISO 639 언어 코드
    QString engName;     // 영명
    QString endonymName; // 현지 이름
};

struct Langs
{
public:
    static LangInfo GetLangInfo(const LangType inLangType);
    static QString GetCodeName(const LangType inLangType);
    static QString GetEnglishName(const LangType inLangType);
    static QString GetEndonymName(const LangType inLangType);
    static QString GetLocaleName(const LangType inLangType);

    static std::vector<LangType> GetLanguageList();

    static bool IsContainName(const LangType inLangType, const QString& inLangName);

private:
    static const std::unordered_map<LangType, LangInfo> langs;
};


enum class LangType
{
    NONE, AUTO, ab, ace, ach, af, sq, alz, am, ar, hy, as, awa, ay, az, ban, bm, ba, eu, btx, bts, bbc, be, bem, bn, bew, bho, bik, bs, br, bg, bua
  , yue, ca, ceb, ny, zh_CN, zh_TW, cv, co, crh, hr, cs, da, din, dv, doi, dov, nl, dz, en, eo, et, ee, fj, fil, fi, fr, fr_FR, fr_CA, fy, ff, gaa, gl
  , lg, ka, de, el, gn, gu, ht, cnh, ha, haw, iw, hil, hi, hmn, hu, hrx, is, ig, ilo, id, ga, it, ja, jw, kn, pam, kk, km, cgg, rw, ktu, gom, ko, kri
  , ku, ckb, ky, lo, ltg, la, lv, lij, li, ln, lt, lmo, luo, lb, mk, mai, mak, mg, ms, ms_Arab, ml, mt, mi, mr, chm, mni, min, lus, mn, my, nr, newa
  , ne, nso, no, nus, oc, ori, om, pag, pap, ps, fa, pl, pt, pt_PT, pt_BR, pa, pa_Arab, qu, rom, ro, rn, ru, sm, sg, sa, gd, sr, st, crs, shn, sn, scn
  , szl, sd, si, sk, sl, so, es, su, sw, ss, sv, tg, ta, tt, te, tet, th, ti, ts, tn, tr, tk, ak, uk, ur, ug, uz, vi, cy, xh, yi, yo, yua, zu, Size
};

enum FinWidgetMode
{
    None         = 0x00000000
  , PopupMode    = 0x00000001
  // , NormalWindow = 0x00000002
  , AlwaysOn     = 0x00000004
};

Q_DECLARE_FLAGS(FinWidgetModeFlags, FinWidgetMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(FinWidgetModeFlags)

enum class OptionPriority
{
    None = 999'999

  , GeneralOption = 0
  , EngineOption

  , AdvancedOption
  
  , Size
};



#endif //FINTYPES_H
