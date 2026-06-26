// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#ifndef SOLTRANSLATOR_SOLMARKDOWNWRITER_H
#define SOLTRANSLATOR_SOLMARKDOWNWRITER_H

#include <QTextDocument>

class QTextList;
class QAbstractItemModel;
class QTextStream;

/**
 * @see qtextmarkdownwriter_p.h
 */
class MarkdownWriter
{
public:
    MarkdownWriter(QTextStream& inStream, QTextDocument::MarkdownFeatures inFeatures);
    bool writeAll(const QTextDocument* inDoc);
    void writeTable(const QAbstractItemModel* inTableModel);

    int writeBlock(const QTextBlock& inBlock, bool inWrap, bool inIgnoreFormat, bool inIgnoreEmpty);
    void writeFrame(const QTextFrame* inFrame);
    void writeFrontMatter(const QString& inFrontMatter);

private:
    struct ListInfo
    {
        bool loose;
    };

    ListInfo listInfo(QTextList* list);
    void setLinePrefixForBlockQuote(int level);
    bool isUseTableCellWidth() const;

private:
    QTextStream& m_stream;
    QTextDocument::MarkdownFeatures m_features;
    QMap<QTextList*, ListInfo> m_listInfo;
    QString m_linePrefix;
    QString m_codeBlockFence;
    int m_wrappedLineIndent     = 0;
    int m_lastListIndent        = 1;
    bool m_doubleNewlineWritten = false;
    bool m_linePrefixWritten    = false;
    bool m_indentedCodeBlock    = false;
    bool m_fencedCodeBlock      = false;
};

#endif //SOLTRANSLATOR_SOLMARKDOWNWRITER_H
