// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLTEXTSPLITER_H
#define SOLTRANSLATOR_SOLTEXTSPLITER_H

#include <QString>

#include <vector>

struct TextChunk
{
    QStringView current;
    QStringView prevContext;
    QStringView nextContext;
};

namespace Sol
{
/**
 * Splits The text at appropriate position.
 * Finds appropriate paragraph and sentence
 * splitting positions around inSplitSize (+-inTolerance).
 */
std::vector<QStringView> splitText(QStringView inText
                                 , const int inSplitSize = 4000
                                 , const int inTolerance = 500);

/**
 * Creates a TextChunk from the text split at appropriate locations.
 * A TextChunk includes portions of the text before and after to understand the context.
 */
std::vector<TextChunk> splitToChunks(QStringView inText
                                   , const int inSplitSize = 4000
                                   , const int inTolerance = 500);
} // namespace Sol


/**
 * The SolTextSplitter is a utility class used for splitting strings into paragraphs.
 * The split result is returned as QStringView.
 * It holds a QString, ensuring that the QString outlives the QStringView.
 */
class SolTextSpliter
{
public:
    SolTextSpliter() = default;

    explicit SolTextSpliter(QStringView inText);

    void setText(QStringView inText);
    QString getText() { return _text; }

    const std::vector<QStringView>& getSplitTexts(const int inSplitSize = 4000
                                                , const int inTolerance = 500);

    const std::vector<TextChunk>& getSplitChunks(const int inSplitSize = 4000
                                               , const int inTolerance = 500);

private:
    QString _text;

    std::vector<QStringView> _splitTexts;
    std::vector<TextChunk> _splitChunks;
};

#endif //SOLTRANSLATOR_SOLTEXTSPLITER_H
