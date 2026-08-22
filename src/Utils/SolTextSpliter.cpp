// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTextSpliter.h"

#include "SolDebug.h"

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
QStringView getFirstSentence(const QStringView inTextView, const int inMinLen = 50, const int inMaxLen = 200)
{
    const int textSize = inTextView.size();
    const int startIdx = std::min<int>(inMinLen, textSize);
    const int endIdx   = std::min<int>(inMaxLen, textSize);
    const int sliceLen = std::max<int>(0, endIdx - startIdx);

    const QStringView slice = inTextView.mid(startIdx, sliceLen);
    static const QRegularExpression re(R"([.!?]\s)");
    const int findIdx = slice.indexOf(re);

    int splitIdx = endIdx;
    if (findIdx != -1)
    {
        splitIdx = startIdx + findIdx + 1;
    }

    return inTextView.left(splitIdx);
}

/**
 * 텍스트에서 마지막 문장을 추출합니다.
 */
QStringView getLastSentence(const QStringView inTextView, const int inMinLen = 50, const int inMaxLen = 200)
{
    const int textSize = inTextView.size();
    const int startIdx = std::max<int>(0, textSize - inMaxLen);
    const int endIdx   = std::max<int>(0, textSize - inMinLen);
    const int sliceLen = std::max<int>(0, endIdx - startIdx);

    const QStringView slice = inTextView.mid(startIdx, sliceLen);
    static const QRegularExpression re(R"([.!?]\s)");
    const int findIdx = slice.lastIndexOf(re);

    int splitIdx = startIdx;
    if (findIdx != -1)
    {
        splitIdx = startIdx + findIdx + 1;
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
        return {inText};
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

std::vector<TextChunk> textsToChunks(const std::vector<QStringView>& inSplitTexts, const int inMinLen, const int inMaxLen)
{
    static const QStringView zeroView = QString("");

    const int splitSize = inSplitTexts.size();

    std::vector<TextChunk> res;
    res.reserve(splitSize);

    for (int i = 0; i < splitSize; ++i)
    {
        QStringView prev = (i > 0) ? getLastSentence(inSplitTexts[i - 1], inMinLen, inMaxLen) : zeroView;
        QStringView next = (i < (splitSize - 1)) ? getFirstSentence(inSplitTexts[i + 1], inMinLen, inMaxLen) : zeroView;

        res.push_back({
            .current     = inSplitTexts[i]
          , .prevContext = std::move(prev)
          , .nextContext = std::move(next)
        });
    }

    return res;
}
} // namespace Sol


SolTextSpliter::SolTextSpliter(QStringView inText
                             , const int inSplitSize
                             , const int inTolerance
                             , const int inMinContextSize
                             , const int inMaxContextSize)
    : _text(inText)
    , _splitSize(inSplitSize)
    , _tolerance(inTolerance)
    , _minContextSize(inMinContextSize)
    , _maxContextSize(inMaxContextSize)
    , _splitTexts({})
    , _splitChunks({})
{}

void SolTextSpliter::resetSplitCaches()
{
    _splitTexts.clear();
    _splitChunks.clear();
}

void SolTextSpliter::setText(QStringView inText)
{
    if (_text == inText)
    {
        return;
    }

    _text = inText.toString();

    resetSplitCaches();
}

void SolTextSpliter::setSplitSize(const int inSplitSize)
{
    if (_splitSize == inSplitSize)
    {
        return;
    }

    _splitSize = inSplitSize;

    resetSplitCaches();
}

void SolTextSpliter::setTolerance(const int inTolerance)
{
    if (_tolerance == inTolerance)
    {
        return;
    }

    _tolerance = inTolerance;

    resetSplitCaches();
}

void SolTextSpliter::setMinContextSize(const int inMinContextSize)
{
    if (_minContextSize == inMinContextSize)
    {
        return;
    }

    _minContextSize = inMinContextSize;

    _splitChunks.clear();
}

void SolTextSpliter::setMaxContextSize(const int inMaxContextSize)
{
    if (_maxContextSize == inMaxContextSize)
    {
        return;
    }

    _maxContextSize = inMaxContextSize;

    _splitChunks.clear();
}

const std::vector<QStringView>& SolTextSpliter::textList()
{
    if (_splitTexts.empty())
    {
        _splitTexts = Sol::splitText(_text, _splitSize, _tolerance);

        _splitChunks.clear();
    }

    return _splitTexts;
}

const std::vector<TextChunk>& SolTextSpliter::chunkList()
{
    if (_splitChunks.empty())
    {
        // Induce _splitTexts generation.
        textList();

        _splitChunks = Sol::textsToChunks(_splitTexts, _minContextSize, _maxContextSize);
    }

    return _splitChunks;
}

QDebug operator<<(QDebug inDebug, const TextChunk& inChunks)
{
    const QString res
            = "prev: " + inChunks.prevContext + "\n"
            + "curr: " + inChunks.current + "\n"
            + "next: " + inChunks.nextContext + "\n";

    QDebugStateSaver saver(inDebug);
    inDebug.nospace().noquote() << res;

    return inDebug;
}

QDebug operator<<(QDebug inDebug, const std::vector<TextChunk>& inChunks)
{
    return Sol::debugRange(inDebug, inChunks);
}

