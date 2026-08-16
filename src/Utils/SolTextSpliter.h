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
                                 , const int inSplitSize
                                 , const int inTolerance);

/**
 * Creates a TextChunk from the text split at appropriate locations.
 * A TextChunk includes portions of the text before and after to understand the context.
 */
std::vector<TextChunk> textsToChunks(const std::vector<QStringView>& inSplitTexts
                                   , const int inMinLen
                                   , const int inMaxLen);
} // namespace Sol


/**
 * The SolTextSplitter is a utility class used for splitting strings into paragraphs.
 * The split result is returned as QStringView.
 * This class holds the text as a 'QString', ensuring that the the outlives the returned QStringView.
 */
class SolTextSpliter
{
public:
    explicit SolTextSpliter(QStringView inText         = u""
                          , const int inSplitSize      = 4000
                          , const int inTolerance      = 500
                          , const int inMinContextSize = 100
                          , const int inMaxContextSize = 200);

    void resetSplitCaches();

    void setText(QStringView inText);
    void setSplitSize(const int inSplitSize);
    void setTolerance(const int inTolerance);
    void setMinContextSize(const int inMinContextSize);
    void setMaxContextSize(const int inMaxContextSize);

    QString getText() const { return _text; }
    int getSplitSize() const { return _splitSize; }
    int getTolerance() const { return _tolerance; }
    int getMinContextSize() const { return _minContextSize; }
    int getMaxContextSize() const { return _maxContextSize; }

    const std::vector<QStringView>& textList();
    const std::vector<TextChunk>& chunkList();

private:
    QString _text;

    int _splitSize;
    int _tolerance;

    int _minContextSize;
    int _maxContextSize;

    std::vector<QStringView> _splitTexts;
    std::vector<TextChunk> _splitChunks;
};

QDebug operator<<(QDebug inDebug, const TextChunk& inChunks);
QDebug operator<<(QDebug inDebug, const std::vector<TextChunk>& inChunks);

#endif //SOLTRANSLATOR_SOLTEXTSPLITER_H
