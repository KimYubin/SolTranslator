// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTYPES_H
#define SOLTYPES_H

#include <QObject>
#include <QPointer>

class QString;
class ITranslateWidget;
enum class EngineType;
enum class LangType;


struct EngineHelper
{
public:
    static QString displayName(const EngineType inEngineType);
    static EngineType defaultEngineType();
};


namespace Sol
{
Q_NAMESPACE

enum HistoryRole
{
    CheckRole = Qt::UserRole + 1
  , SourceLangRole
  , TagetLangRole
  , SourceSimplifiedTextRole
  , TargetSimplifiedTextRole

  , TimeStampRole

  , Size
};

enum ItemColorRole
{
    itemTextColorRole
  , itemSelectionTextColorRole
  , itemHoverTextColorRole
  , itemDisableColorRole
};

} // namespace Sol

enum class ScreenPopupPolicy
{
    Default

  , PrimaryScreen
  , FixedScreen
  , CursorScreen

  , Size
};


enum class EngineType
{
    Google
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

enum class TextType
{
    SourceText
  , TargetText
};

struct TranslateRequestInfo
{
    TranslateRequestInfo() = default;

    /**
     * 번역 요청에 필요한 정보를 모아놓은 구조체입니다.
     * 
     * @param inTrDisplayWidget trUnit의 번역값을 표기하는 ITranslateWidget입니다.
     * @param inIsIgnoreCache true면 캐시에서 찾지 않고 번역을 요청합니다. 재번역시 사용됩니다.
     * @param inEngineType 번역엔진 종류. 엔진 종류가 다르면, 기록에서도 다른 번역으로 취급됩니다.
     * @param inSourceText 번역 원문
     * @param inTextFormat 원문 텍스트의 종류.
     * @param inSourceLang 출발 언어
     * @param inTargetLang 도착 언어
     * @param inCompleteContext callbackTranslateComplete 수명을 관리하는 객체입니다.
     * @param inFuncComplete 번역이 완료되면 호출되는 콜백입니다.
     * @param inStreamContext callbackTranslateStreaming 수명을 관리하는 객체입니다.
     * @param inFuncStreaming 번역 스트리밍 중간 값들을 받는 콜백입니다. 여러번 호출됩니다.
     */
    TranslateRequestInfo(ITranslateWidget* inTrDisplayWidget
                       , const bool inIsIgnoreCache
                       , const EngineType inEngineType
                       , const QString& inSourceText
                       , const TextStyle inTextFormat
                       , const LangType inSourceLang
                       , const LangType inTargetLang
                       , QObject* inCompleteContext
                       , std::move_only_function<void(const QString&)>&& inFuncComplete
                       , QObject* inStreamContext = nullptr
                       , std::optional<std::move_only_function<void(const QString&)>>&& inFuncStreaming = std::nullopt);

    QPointer<ITranslateWidget> trDisplayWidget;
    bool isIgnoreCache;
    EngineType engineType;
    QString sourceText;
    TextStyle textFormat;
    LangType sourceLang;
    LangType targetLang;
    QPointer<QObject> completeContext;
    std::move_only_function<void(const QString&)> callbackTranslateComplete;
    QPointer<QObject> streamContext;
    std::optional<std::move_only_function<void(const QString&)>>  callbackTranslateStreaming;
};


enum class Action
{
    None

  , PopupTranslate

  , SettingsOpen
  , SettingsClose
  , MainClose
  , MainNextTab
  , MainPrevTab
  , SourceTargetToggle
  , CopyDoc
  , ViewInPopup
  , ReTranslate
  , DeleteTranslation

  , PopupAlwaysOn
  , PopupWindowMode
  , PopupMinimize
  , PopupMaxRestore

  , PopupClose

  , Size
};


/**
 * 언어 타입, 코드, 이름 등을 총괄합니다
 * todo: QLocale로 개선해야합니다.
 */
struct LangInfo
{
public:
    LangInfo(const LangType inLangType, const QString& inCodeName, const QString& inEngName, const QString& inEndonymName)
        : langType(inLangType)
        , codeName(inCodeName)
        , engName(inEngName)
        , endonymName(inEndonymName)
    {}


    LangType langType;   // 언어 타입
    QString codeName;    // ISO 639 언어 코드
    QString engName;     // 영명
    QString endonymName; // 현지 이름
};

struct Langs
{
public:
    static LangInfo getLangInfo(const LangType inLangType);
    static QString getCodeName(const LangType inLangType);
    static QString getEnglishName(const LangType inLangType);
    static QString getEndonymName(const LangType inLangType);
    static QString getLocaleName(const LangType inLangType);

    static std::vector<LangType> getLanguageList();

    static bool containName(const LangType inLangType, const QString& inLangName);

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

enum SolWidgetMode
{
    None         = 0x00000000
  , PopupMode    = 0x00000001
  // , NormalWindow = 0x00000002
  , AlwaysOn     = 0x00000004
};

Q_DECLARE_FLAGS(SolWidgetModeFlags, SolWidgetMode)
Q_DECLARE_OPERATORS_FOR_FLAGS(SolWidgetModeFlags)

enum class OptionPriority
{
    None = 999'999

  , GeneralOption = 0
  , EngineOption

  , AdvancedOption
  
  , Size
};



#endif //SOLTYPES_H
