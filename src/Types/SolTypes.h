// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTYPES_H
#define SOLTYPES_H
#include <functional>

#include <QString>
#include <qnamespace.h>

class QString;
class ITranslateWidget;
enum class LangType;


template <typename Signature>
using Callback = std::move_only_function<Signature>;


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



/**
 * This enum class distinguishes the rendering styles of text, such as plain text, HTML, and markdown.
 */
enum class TextStyle
{
    None
  , PlainText
  , Html
  , MarkDown
  , Size
};

/**
 * This enum class distinguishes between Source text and Target text.
 */
enum class TextCategory
{
    SourceText
  , TargetText
};


enum class Action
{
    None

  , MainWidgetRaise
  , PopupTranslate

  , SettingsOpen
  , SettingsClose

  , MainClose
  , MainNextTab
  , MainPrevTab
  , TextTab
  , DocumentTab
  , HistoryTab

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


#endif //SOLTYPES_H
