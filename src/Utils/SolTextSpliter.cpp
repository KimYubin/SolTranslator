// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTextSpliter.h"

#include <QRegularExpression>
#include <QString>

#include <algorithm>
#include <vector>

namespace
{
/**
 * 주어진 구분자들의 모든 변형(마침표, 느낌표, 물음표)을 생성합니다.
 */
std::vector<QString> endMarkerVars(const std::vector<QString>& inTemplates)
{
    const std::array<QString, 6> endMarks = {".", "!", "?", "。", "｡", "︒"};

    std::vector<QString> res;
    res.reserve(inTemplates.size() * endMarks.size() * 2);

    for (const QString& str : inTemplates)
    {
        for (const QString& endMk : endMarks)
        {
            res.push_back(str.arg(endMk, " "));
            res.push_back(str.arg(endMk, ""));
        }
    }

    return res;
}

/**
 * 우선순위에 따라 정렬된 구분자 리스트 (정적 초기화)
 */
const std::vector<QString>& getSeparators()
{
    static const std::vector<QString> separatorList = []()
    {
        const QString endPh   = "%1";
        const QString spacePh = "%2";

        std::vector<QString> res;

        // 1순위: 문단 구분
        res.append_range(endMarkerVars({endPh + spacePh + "\n\n"}));
        res.emplace_back("\n\n");

        // 2순위: 문장 종료 + 줄바꿈
        res.append_range(endMarkerVars({endPh + spacePh + "\n"}));
        res.emplace_back("\n");

        // 3순위: 문장 종료 (문단 내). 따옴표 닫힘 포함.
        res.append_range(endMarkerVars({endPh + spacePh, endPh + "\"" + spacePh, endPh + "\'" + spacePh}));

        // 4순위: 약한 구분자
        res.append_range(std::vector<QString>{"…", ";\n", ";", ":\n", ":"});
        return res;
    }();

    return separatorList;
}

/**
 * 텍스트에서 첫 문장을 추출합니다.
 */
QStringView getFirstSentence(const QStringView inTextView, const int inMaxLen = 200, const int inMinLen = 100)
{
    const int sliceMin = std::min<int>(inMinLen, inTextView.size());
    const int sliceMax = std::min<int>(inMaxLen, inTextView.size());
    const int sliceLen = std::max<int>(0, sliceMax - sliceMin);

    const QStringView slice = inTextView.mid(sliceMin, sliceLen);
    const QRegularExpression re(R"([.!?]\s)");
    const int nextPunctuationIdx = slice.indexOf(re);

    int splitIdx;
    if (nextPunctuationIdx == -1)
    {
        splitIdx = sliceMin + inMaxLen + 1; // 종결 부호가 없을 때 JS 원본과 동일하게 강제 인덱싱 계산
    }
    else
    {
        splitIdx = sliceMin + nextPunctuationIdx + 1;
    }

    return inTextView.left(splitIdx); // Qt에서는 길이가 초과해도 안전하게 끝까지 반환함
}

/**
 * 텍스트에서 마지막 문장을 추출합니다.
 */
QStringView getLastSentence(const QStringView inTextView, const int inMaxLen = 200, const int inMinLen = 100)
{
    const int textLen  = inTextView.size();
    const int startIdx = std::max<int>(0, textLen - inMaxLen);
    const int endIdx   = std::max<int>(0, textLen - inMinLen);
    const int sliceLen = std::max<int>(0, endIdx - startIdx);

    const QStringView slice = inTextView.mid(startIdx, sliceLen);
    const QRegularExpression re(R"([.!?]\s)");
    const int nextPunctuationIdx = slice.indexOf(re);

    int splitIdx = startIdx;
    if (nextPunctuationIdx != -1)
    {
        splitIdx = startIdx + nextPunctuationIdx + 1;
    }

    return inTextView.mid(splitIdx);
}
} // anonymous namespace


namespace Sol
{
std::vector<QStringView> splitText(QStringView inText
                                 , const int inSplitSize
                                 , const int inTolerance)
{
    const int minLength = inSplitSize - inTolerance;
    const int maxLength = inSplitSize + inTolerance;

    if (inText.isEmpty() || inText.size() <= maxLength)
    {
        return {{inText, inText.first(0), inText.first(0)}};
    }

    std::vector<QStringView> splitTexts;

    const int loopLimit = std::max<int>(10'000, (inText.size() / minLength) + 1);
    int loopCount{0};

    while (!inText.isEmpty())
    {
        if (loopCount++ > loopLimit)
        {
            while (!inText.isEmpty())
            {
                splitTexts.push_back(inText.first(inSplitSize));
                inText.chop(inSplitSize);
            }
            break;
        }

        if (inText.size() <= maxLength)
        {
            splitTexts.push_back(inText);
            break;
        }

        int splitIndex = -1;

        for (const QString& seperatorPattern : getSeparators())
        {
            const int safeMaxLen = std::min<int>(inText.size(), maxLength);
            QStringView subView  = QStringView(inText).sliced(0, safeMaxLen);

            const int findIdx = subView.indexOf(seperatorPattern, minLength);
            if (0 <= findIdx && findIdx <= maxLength)
            {
                splitIndex = findIdx + seperatorPattern.size();
                break;
            }
        }

        if (splitIndex == -1)
        {
            int altIdx = inText.lastIndexOf(' ', maxLength);
            if (altIdx < minLength)
            {
                altIdx = std::min<int>(maxLength, inText.size());
            }

            splitIndex = altIdx;
        }

        if (splitIndex <= 0)
        {
            splitIndex = std::min<int>(maxLength, inText.size());
        }

        splitIndex = std::min<int>(splitIndex, inText.size());

        splitTexts.push_back(inText.left(splitIndex));
        inText = inText.mid(splitIndex);
    }


    return splitTexts;
}

std::vector<TextChunk> splitToChunks(QStringView inText, const int inSplitSize, const int inTolerance)
{
    static const QStringView zeroView = QString("");

    const std::vector<QStringView> splitList = splitText(inText, inSplitSize, inTolerance);

    std::vector<TextChunk> res;
    res.reserve(splitList.size());

    for (int i = 0; i < splitList.size(); ++i)
    {
        QStringView prev = (i > 0) ? getLastSentence(splitList[i - 1]) : zeroView;
        QStringView next = (i < (splitList.size() - 1)) ? getFirstSentence(splitList[i + 1]) : zeroView;

        res.push_back({
            .current     = splitList[i]
          , .prevContext = std::move(prev)
          , .nextContext = std::move(next)
        });
    }

    return res;
}
} // namespace Sol


SolTextSpliter::SolTextSpliter(QStringView inText)
    : _text(inText)
    , _splitTexts({})
    , _splitChunks({})
{}

void SolTextSpliter::setText(QStringView inText)
{
    _text = inText.toString();
    _splitTexts.clear();
    _splitChunks.clear();
}

const std::vector<QStringView>& SolTextSpliter::getSplitTexts(const int inSplitSize, const int inTolerance)
{
    if (_splitTexts.empty())
    {
        _splitTexts = Sol::splitText(_text, inSplitSize, inTolerance);
    }

    return _splitTexts;
}

const std::vector<TextChunk>& SolTextSpliter::getSplitChunks(const int inSplitSize, const int inTolerance)
{
    if (_splitChunks.empty())
    {
        _splitChunks = Sol::splitToChunks(_text, inSplitSize, inTolerance);
    }

    return _splitChunks;
}
