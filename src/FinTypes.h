//
// Created by YubinKim on 25/03/11 화.
//

#ifndef FINTYPES_H
#define FINTYPES_H
#include <QString>


class FinTypes
{
};

struct EngineName
{
public:
    static const QString OPEN_AI;
};


/** 보편 경로 */
struct StaticPath
{
public:
    static const QString API_KEY_PATH;
};

struct StaticPrompt
{
public:
    static const QString OPEN_AI_PROMPT;
};

struct LangInfo
{
public:
    LangInfo(const QString& InName, const QString& InCode): Name(InName), Code(InCode)    {
    };
    QString Name;
    QString Code;
};

struct Langs
{
public:
    static const LangInfo ENGLISH;
    static const LangInfo KOREAN;
};


enum class Lang
{
    NONE, AUTO, ab, ace, ach, af, sq, alz, am, ar, hy, as, awa, ay, az, ban, bm, ba, eu, btx, bts, bbc, be, bem, bn, bew, bho, bik, bs, br, bg, bua
  , yue, ca, ceb, ny, zh_CN, zh_TW, cv, co, crh, hr, cs, da, din, dv, doi, dov, nl, dz, en, eo, et, ee, fj, fil, fi, fr, fr_FR, fr_CA, fy, ff, gaa, gl
  , lg, ka, de, el, gn, gu, ht, cnh, ha, haw, iw, hil, hi, hmn, hu, hrx, is, ig, ilo, id, ga, it, ja, jw, kn, pam, kk, km, cgg, rw, ktu, gom, ko, kri
  , ku, ckb, ky, lo, ltg, la, lv, lij, li, ln, lt, lmo, luo, lb, mk, mai, mak, mg, ms, ms_Arab, ml, mt, mi, mr, chm, mni, min, lus, mn, my, nr, newa
  , ne, nso, no, nus, oc, ori, om, pag, pap, ps, fa, pl, pt, pt_PT, pt_BR, pa, pa_Arab, qu, rom, ro, rn, ru, sm, sg, sa, gd, sr, st, crs, shn, sn, scn
  , szl, sd, si, sk, sl, so, es, su, sw, ss, sv, tg, ta, tt, te, tet, th, ti, ts, tn, tr, tk, ak, uk, ur, ug, uz, vi, cy, xh, yi, yo, yua, zu, SIZE
};



#endif //FINTYPES_H
