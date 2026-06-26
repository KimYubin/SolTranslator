// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "SolMarkdownWriter.h"

#include <QTextDocument>
#include <QtCore/QTextStream>

#include "qfontinfo.h"
#include "qfontmetrics.h"
#include "qtextlist.h"
#include "qtexttable.h"
#include "qtextcursor.h"
#include "qloggingcategory.h"
#include <QtCore/QRegularExpression>
#include "qabstractitemmodel.h"
#include "SolLog.h"

#include <QAbstractItemModel>

using namespace Qt::StringLiterals;


namespace
{
Q_LOGGING_CATEGORY(lcMDW, "qt.text.markdown.writer")

constexpr QChar qtmw_Space          = u' ';
constexpr QChar qtmw_Tab            = u'\t';
constexpr QChar qtmw_Newline        = u'\n';
constexpr QChar qtmw_CarriageReturn = u'\r';
constexpr QChar qtmw_LineBreak      = u'\x2028';
constexpr QChar qtmw_DoubleQuote    = u'"';
constexpr QChar qtmw_Backtick       = u'`';
constexpr QChar qtmw_Backslash      = u'\\';
constexpr QChar qtmw_Period         = u'.';
} // anonymous namespace


MarkdownWriter::MarkdownWriter(QTextStream& inStream, QTextDocument::MarkdownFeatures inFeatures)
    : m_stream(inStream), m_features(inFeatures)
{
}

bool MarkdownWriter::writeAll(const QTextDocument* inDoc)
{
    writeFrontMatter(inDoc->metaInformation(QTextDocument::FrontMatter));
    writeFrame(inDoc->rootFrame());
    return true;
}

void MarkdownWriter::writeTable(const QAbstractItemModel* inTableModel)
{
    QList<int> tableColumnWidths(inTableModel->columnCount());
    for (int col = 0; col < inTableModel->columnCount(); ++col)
    {
        tableColumnWidths[col] = inTableModel->headerData(col, Qt::Horizontal).toString().size();
        for (int row = 0; row < inTableModel->rowCount(); ++row)
        {
            tableColumnWidths[col] = qMax(tableColumnWidths[col],
                                          inTableModel->data(inTableModel->index(row, col)).toString().size());
        }
    }

    // write the header and separator
    for (int col = 0; col < inTableModel->columnCount(); ++col)
    {
        QString s = inTableModel->headerData(col, Qt::Horizontal).toString();
        m_stream << '|' << s << QString(tableColumnWidths[col] - s.size(), qtmw_Space);
    }
    m_stream << "|" << Qt::endl;
    for (int col = 0; col < tableColumnWidths.size(); ++col)
    {
        m_stream << '|' << QString(tableColumnWidths[col], u'-');
    }
    m_stream << '|' << Qt::endl;

    // write the body
    for (int row = 0; row < inTableModel->rowCount(); ++row)
    {
        for (int col = 0; col < inTableModel->columnCount(); ++col)
        {
            QString s = inTableModel->data(inTableModel->index(row, col)).toString();
            m_stream << '|' << s << QString(tableColumnWidths[col] - s.size(), qtmw_Space);
        }
        m_stream << '|' << Qt::endl;
    }
    m_listInfo.clear();
}


void MarkdownWriter::writeFrontMatter(const QString& inFrontMatter)
{
    const bool featureEnabled = m_features.testFlag(static_cast<QTextDocument::MarkdownFeature>(0x100000));
    qCDebug(lcMDW) << "writing FrontMatter?" << featureEnabled << "size" << inFrontMatter.size();
    if (inFrontMatter.isEmpty() || !featureEnabled)
    {
        return;
    }
    m_stream << "---\n"_L1 << inFrontMatter;
    if (!inFrontMatter.endsWith(qtmw_Newline))
    {
        m_stream << qtmw_Newline;
    }
    m_stream << "---\n"_L1;
}

void MarkdownWriter::writeFrame(const QTextFrame* inFrame)
{
    Q_ASSERT(inFrame);
    const QTextTable* table       = qobject_cast<const QTextTable*>(inFrame);
    QTextFrame::iterator iterator = inFrame->begin();
    QTextFrame* child             = nullptr;
    int tableRow                  = -1;
    bool lastWasList              = false;
    QList<int> tableColumnWidths;
    if (table)
    {
        solDebug << table->columns() << table->rows();

        tableColumnWidths.resize(table->columns(), 0);
        for (int col = 0; col < table->columns(); ++col)
        {
            if (isUseTableCellWidth() == false)
            {
                continue;
            }
            for (int row = 0; row < table->rows(); ++row)
            {
                QTextTableCell cell = table->cellAt(row, col);
                int cellTextLen     = 0;
                auto it             = cell.begin();
                while (it != cell.end())
                {
                    QTextBlock block = it.currentBlock();
                    if (block.isValid())
                    {
                        cellTextLen += block.text().size();
                    }
                    ++it;
                }
                if (cell.columnSpan() == 1 && tableColumnWidths[col] < cellTextLen)
                {
                    tableColumnWidths[col] = cellTextLen;
                }
            }
        }
    }
    while (!iterator.atEnd())
    {
        if (iterator.currentFrame() && child != iterator.currentFrame())
        {
            writeFrame(iterator.currentFrame());
        }
        else
        {
            // no frame, it's a block
            QTextBlock curBlock = iterator.currentBlock();
            if (curBlock.isValid() == false || (table && (curBlock.length() == 0)))
            {
                continue;
            }

            // Look ahead and detect some cases when we should
            // suppress needless blank lines, when there will be a big change in block format
            bool nextIsDifferent     = false;
            bool ending              = false;
            int blockQuoteIndent     = 0;
            int nextBlockQuoteIndent = 0;
            {
                QTextFrame::iterator next = iterator;
                ++next;
                QTextBlockFormat format     = iterator.currentBlock().blockFormat();
                QTextBlockFormat nextFormat = next.currentBlock().blockFormat();
                blockQuoteIndent            = format.intProperty(QTextFormat::BlockQuoteLevel);
                nextBlockQuoteIndent        = nextFormat.intProperty(QTextFormat::BlockQuoteLevel);
                if (next.atEnd())
                {
                    nextIsDifferent = true;
                    ending          = true;
                }
                else
                {
                    if (nextFormat.indent() != format.indent()
                        || (nextFormat.property(QTextFormat::BlockCodeLanguage) != format.property(QTextFormat::BlockCodeLanguage)))
                    {
                        nextIsDifferent = true;
                    }
                }
            }
            if (table)
            {
                QTextTableCell cell = table->cellAt(curBlock.position());
                if (tableRow < cell.row())
                {
                    if (tableRow == 0)
                    {
                        m_stream << qtmw_Newline;
                        for (int col = 0; col < tableColumnWidths.size(); ++col)
                        {
                            m_stream << '|' << QString(qMax(tableColumnWidths[col], 3), u'-');
                        }
                        m_stream << '|';
                    }
                    m_stream << qtmw_Newline << '|';
                    tableRow = cell.row();
                }
            }
            else if (!curBlock.textList())
            {
                if (lastWasList)
                {
                    m_stream << qtmw_Newline;
                    m_linePrefixWritten = false;
                }
            }

            const int endingCol = writeBlock(curBlock
                                           , !table
                                           , table && tableRow == 0
                                           , nextIsDifferent && !curBlock.textList());

            m_doubleNewlineWritten = false;
            if (table)
            {
                QTextTableCell cell  = table->cellAt(curBlock.position());
                int paddingLen       = -endingCol;
                const int spanEndCol = cell.column() + cell.columnSpan();
                for (int col = cell.column(); col < spanEndCol; ++col)
                {
                    paddingLen += tableColumnWidths[col];
                }
                if (paddingLen > 0)
                {
                    m_stream << QString(paddingLen, qtmw_Space);
                }
                for (int col = cell.column(); col < spanEndCol; ++col)
                {
                    m_stream << "|";
                }
            }
            else if (m_fencedCodeBlock && ending)
            {
                m_stream << qtmw_Newline << m_linePrefix << QString(m_wrappedLineIndent, qtmw_Space)
                        << m_codeBlockFence << qtmw_Newline << qtmw_Newline;
                m_codeBlockFence.clear();
            }
            else if (m_indentedCodeBlock && nextIsDifferent)
            {
                m_stream << qtmw_Newline << qtmw_Newline;
            }
            else if (endingCol > 0)
            {
                if (curBlock.textList() || curBlock.blockFormat().hasProperty(QTextFormat::BlockCodeLanguage))
                {
                    m_stream << qtmw_Newline;
                    if (curBlock.textList())
                    {
                        m_stream << m_linePrefix;
                        m_linePrefixWritten = true;
                    }
                }
                else
                {
                    m_stream << qtmw_Newline;
                    if (nextBlockQuoteIndent < blockQuoteIndent)
                    {
                        setLinePrefixForBlockQuote(nextBlockQuoteIndent);
                    }
                    m_stream << m_linePrefix;
                    m_stream << qtmw_Newline;
                    m_doubleNewlineWritten = true;
                }
            }
            lastWasList = curBlock.textList();
        }
        child = iterator.currentFrame();
        ++iterator;
    }
    if (table)
    {
        m_stream << qtmw_Newline << qtmw_Newline;
        m_doubleNewlineWritten = true;
    }
    m_listInfo.clear();
}

MarkdownWriter::ListInfo MarkdownWriter::listInfo(QTextList* list)
{
    if (!m_listInfo.contains(list))
    {
        // decide whether this list is loose or tight
        ListInfo info;
        info.loose = false;
        if (list->count() > 1)
        {
            QTextBlock first = list->item(0);
            QTextBlock last  = list->item(list->count() - 1);
            QTextBlock next  = first.next();
            while (next.isValid())
            {
                if (next == last)
                {
                    break;
                }
                qCDebug(lcMDW) << "next block in list" << list << next.text() << "part of list?" << next.textList();
                if (!next.textList())
                {
                    // If we find a continuation paragraph, this list is "loose"
                    // because it will need a blank line to separate that paragraph.
                    qCDebug(lcMDW) << "decided list beginning with" << first.text() << "is loose after" << next.text();
                    info.loose = true;
                    break;
                }
                next = next.next();
            }
        }
        m_listInfo.insert(list, info);
        return info;
    }
    return m_listInfo.value(list);
}

void MarkdownWriter::setLinePrefixForBlockQuote(int level)
{
    m_linePrefix.clear();
    if (level > 0)
    {
        m_linePrefix.reserve(level * 2);
        for (int i = 0; i < level; ++i)
        {
            m_linePrefix += u"> ";
        }
    }
}

bool MarkdownWriter::isUseTableCellWidth() const
{
    return false;
}

namespace
{

int nearestWordWrapIndex(const QString& s, int inBefore)
{
    inBefore            = qMin(inBefore, s.size());
    const int fragBegin = qMax(inBefore - 15, 0);
    if (lcMDW().isDebugEnabled())
    {
        const QString frag = s.mid(fragBegin, 30);
        qCDebug(lcMDW) << frag << inBefore;
        qCDebug(lcMDW) << QString(inBefore - fragBegin, qtmw_Period) + u'<';
    }
    for (int i = inBefore - 1; i >= 0; --i)
    {
        if (s.at(i).isSpace())
        {
            qCDebug(lcMDW) << QString(i - fragBegin, qtmw_Period) + u'^' << i;
            return i;
        }
    }
    qCDebug(lcMDW, "not possible");
    return -1;
}

int adjacentBackticksCount(const QString& s)
{
    int start = -1, len = s.size();
    int ret   = 0;
    for (int i = 0; i < len; ++i)
    {
        if (s.at(i) == qtmw_Backtick)
        {
            if (start < 0)
            {
                start = i;
            }
        }
        else if (start >= 0)
        {
            ret   = qMax(ret, i - start);
            start = -1;
        }
    }
    if (s.at(len - 1) == qtmw_Backtick)
    {
        ret = qMax(ret, len - start);
    }
    return ret;
}

/*! \internal
    Escape anything at the beginning of a line of markdown that would be
    misinterpreted by a markdown parser, including any period that follows a
    number (to avoid misinterpretation as a numbered list item).
    https://spec.commonmark.org/0.31.2/#backslash-escapes
*/
void maybeEscapeFirstChar(QString& s)
{
    static const QRegularExpression numericListRe(uR"(\d+([\.)])\s)"_s);
    static const QLatin1StringView specialFirstCharacters("#*+-");

    const QString sTrimmed = s.trimmed();
    if (sTrimmed.isEmpty())
    {
        return;
    }
    const QChar firstChar = sTrimmed.at(0);
    if (specialFirstCharacters.contains(firstChar))
    {
        const int i = s.indexOf(firstChar); // == 0 unless s got trimmed
        s.insert(i, u'\\');
    }
    else
    {
        const QRegularExpressionMatch match = numericListRe.match(s, 0, QRegularExpression::NormalMatch
                                                                , QRegularExpression::AnchorAtOffsetMatchOption);
        if (match.hasMatch())
        {
            s.insert(match.capturedStart(1), qtmw_Backslash);
        }
    }
}

/*! \internal
    Escape all backslashes. Then escape any special character that stands
    alone or prefixes a "word", including the \c < that starts an HTML tag.
    https://spec.commonmark.org/0.31.2/#backslash-escapes
*/
void escapeSpecialCharacters(QString& s)
{
    static const QRegularExpression spaceRe(uR"(\s+)"_s);
    static const QRegularExpression specialRe(uR"([<!*[`&]+[/\w])"_s);

    s.replace("\\"_L1, "\\\\"_L1);

    int i = 0;
    while (i >= 0)
    {
        if (int j = s.indexOf(specialRe, i); j >= 0)
        {
            s.insert(j, qtmw_Backslash);
            i = j + 3;
        }
        i = s.indexOf(spaceRe, i);
        if (i >= 0)
        {
            ++i; // past the whitespace, if found
        }
    }
}

struct LineEndPositions
{
    const QChar* lineEnd;
    const QChar* nextLineBegin;
};

LineEndPositions findLineEnd(const QChar* begin, const QChar* end)
{
    LineEndPositions result{end, end};

    while (begin < end)
    {
        if (*begin == qtmw_Newline)
        {
            result.lineEnd       = begin;
            result.nextLineBegin = begin + 1;
            break;
        }
        else if (*begin == qtmw_CarriageReturn)
        {
            result.lineEnd       = begin;
            result.nextLineBegin = begin + 1;
            if (((begin + 1) < end) && begin[1] == qtmw_Newline)
            {
                ++result.nextLineBegin;
            }
            break;
        }

        ++begin;
    }

    return result;
}

bool isBlankLine(const QChar* begin, const QChar* end)
{
    while (begin < end)
    {
        if (*begin != qtmw_Space && *begin != qtmw_Tab)
        {
            return false;
        }
        ++begin;
    }
    return true;
}

QString createLinkTitle(const QString& inTitle)
{
    QString result;
    result.reserve(inTitle.size() + 2);
    result += qtmw_DoubleQuote;

    const QChar* data = inTitle.data();
    const QChar* end  = data + inTitle.size();

    while (data < end)
    {
        const auto lineEndPositions = findLineEnd(data, end);

        if (!isBlankLine(data, lineEndPositions.lineEnd))
        {
            while (data < lineEndPositions.nextLineBegin)
            {
                if (*data == qtmw_DoubleQuote)
                {
                    result += qtmw_Backslash;
                }
                result += *data;
                ++data;
            }
        }

        data = lineEndPositions.nextLineBegin;
    }

    result += qtmw_DoubleQuote;
    return result;
}

} // anonymous namespace

int MarkdownWriter::writeBlock(const QTextBlock& inBlock, bool inWrap, bool inIgnoreFormat, bool inIgnoreEmpty)
{
    if (inBlock.text().isEmpty() && inIgnoreEmpty)
    {
        return 0;
    }
    constexpr int ColumnLimit     = 1'000'000;
    QTextBlockFormat blockFmt     = inBlock.blockFormat();
    bool missedBlankCodeBlockLine = false;
    const bool codeBlock          = blockFmt.hasProperty(QTextFormat::BlockCodeFence) ||
            blockFmt.stringProperty(QTextFormat::BlockCodeLanguage).size() > 0 ||
            blockFmt.nonBreakableLines();
    const int blockQuoteLevel = blockFmt.intProperty(QTextFormat::BlockQuoteLevel);
    if (m_fencedCodeBlock && !codeBlock)
    {
        m_stream << m_linePrefix << m_codeBlockFence << qtmw_Newline;
        m_fencedCodeBlock = false;
        m_codeBlockFence.clear();
        m_linePrefixWritten = m_linePrefix.size() > 0;
    }
    m_linePrefix.clear();
    if (!blockFmt.headingLevel() && blockQuoteLevel > 0)
    {
        setLinePrefixForBlockQuote(blockQuoteLevel);
        if (!m_linePrefixWritten)
        {
            m_stream << m_linePrefix;
            m_linePrefixWritten = true;
        }
    }
    if (inBlock.textList())
    {
        // it's a list-item
        auto fmt            = inBlock.textList()->format();
        const int listLevel = fmt.indent();
        // Negative numbers don't start a list in Markdown, so ignore them.
        const int start   = fmt.start() >= 0 ? fmt.start() : 1;
        const int number  = inBlock.textList()->itemNumber(inBlock) + start;
        QByteArray bullet = " ";
        bool numeric      = false;
        switch (fmt.style())
        {
        case QTextListFormat::ListDisc:
            bullet = "-";
            m_wrappedLineIndent = 2;
            break;
        case QTextListFormat::ListCircle:
            bullet = "*";
            m_wrappedLineIndent = 2;
            break;
        case QTextListFormat::ListSquare:
            bullet = "+";
            m_wrappedLineIndent = 2;
            break;
        case QTextListFormat::ListStyleUndefined: break;
        case QTextListFormat::ListDecimal:
        case QTextListFormat::ListLowerAlpha:
        case QTextListFormat::ListUpperAlpha:
        case QTextListFormat::ListLowerRoman:
        case QTextListFormat::ListUpperRoman:
            numeric = true;
            m_wrappedLineIndent = 4;
            break;
        }
        switch (blockFmt.marker())
        {
        case QTextBlockFormat::MarkerType::Checked:
            bullet += " [x]";
            break;
        case QTextBlockFormat::MarkerType::Unchecked:
            bullet += " [ ]";
            break;
        default:
            break;
        }
        int indentFirstLine = (listLevel - 1) * (numeric ? 4 : 2);
        m_wrappedLineIndent += indentFirstLine;
        if (m_lastListIndent != listLevel && !m_doubleNewlineWritten && listInfo(inBlock.textList()).loose)
        {
            m_stream << qtmw_Newline;
        }
        m_lastListIndent = listLevel;
        QString prefix(indentFirstLine, qtmw_Space);
        if (numeric)
        {
            QString suffix = fmt.numberSuffix();
            if (suffix.isEmpty())
            {
                suffix = QString(qtmw_Period);
            }
            QString numberStr = QString::number(number) + suffix + qtmw_Space;
            if (numberStr.size() == 3)
            {
                numberStr += qtmw_Space;
            }
            prefix += numberStr;
        }
        else
        {
            prefix += QLatin1StringView(bullet) + qtmw_Space;
        }
        m_stream << prefix;
    }
    else if (blockFmt.hasProperty(QTextFormat::BlockTrailingHorizontalRulerWidth))
    {
        m_stream << "- - -\n"; // unambiguous horizontal rule, not an underline under a heading
        return 0;
    }
    else if (codeBlock)
    {
        // It's important to preserve blank lines in code blocks.  But blank lines in code blocks
        // inside block quotes are getting preserved anyway (along with the "> " prefix).
        if (!blockFmt.hasProperty(QTextFormat::BlockQuoteLevel))
        {
            missedBlankCodeBlockLine = true; // only if we don't get any fragments below
        }
        if (!m_fencedCodeBlock)
        {
            QString fenceChar = blockFmt.stringProperty(QTextFormat::BlockCodeFence);
            if (fenceChar.isEmpty())
            {
                fenceChar = "`"_L1;
            }
            m_codeBlockFence = QString(3, fenceChar.at(0));
            if (blockFmt.hasProperty(QTextFormat::BlockIndent))
            {
                m_codeBlockFence = QString(m_wrappedLineIndent, qtmw_Space) + m_codeBlockFence;
            }
            // A block quote can contain an indented code block, but not vice-versa.
            m_stream << m_codeBlockFence << blockFmt.stringProperty(QTextFormat::BlockCodeLanguage)
                    << qtmw_Newline << m_linePrefix;
            m_fencedCodeBlock = true;
        }
        inWrap = false;
    }
    else if (!blockFmt.indent())
    {
        m_wrappedLineIndent = 0;
        if (blockFmt.hasProperty(QTextFormat::BlockCodeLanguage))
        {
            // A block quote can contain an indented code block, but not vice-versa.
            m_linePrefix        += QString(4, qtmw_Space);
            m_indentedCodeBlock = true;
        }
        if (!m_linePrefixWritten)
        {
            m_stream << m_linePrefix;
            m_linePrefixWritten = true;
        }
    }
    if (blockFmt.headingLevel())
    {
        m_stream << QByteArray(blockFmt.headingLevel(), '#') << ' ';
        inWrap = false;
    }

    QString wrapIndentString = m_linePrefix + QString(m_wrappedLineIndent, qtmw_Space);
    // It would be convenient if QTextStream had a lineCharPos() accessor,
    // to keep track of how many characters (not bytes) have been written on the current line,
    // but it doesn't.  So we have to keep track with this col variable.
    int col                       = wrapIndentString.size();
    bool mono                     = false;
    bool startsOrEndsWithBacktick = false;
    bool bold                     = false;
    bool italic                   = false;
    bool underline                = false;
    bool strikeOut                = false;
    bool endingMarkers            = false;
    QString backticks(qtmw_Backtick);
    for (QTextBlock::Iterator frag = inBlock.begin(); !frag.atEnd(); ++frag)
    {
        missedBlankCodeBlockLine = false;
        QString fragmentText     = frag.fragment().text();
        while (fragmentText.endsWith(qtmw_Newline))
        {
            fragmentText.chop(1);
        }
        if (!(m_fencedCodeBlock || m_indentedCodeBlock))
        {
            escapeSpecialCharacters(fragmentText);
            maybeEscapeFirstChar(fragmentText);
        }
        if (inBlock.textList())
        {
            // <li>first line</br>continuation</li>
            QString newlineIndent =
                    QString(qtmw_Newline) + QString(m_wrappedLineIndent, qtmw_Space);
            fragmentText.replace(QString(qtmw_LineBreak), newlineIndent);
        }
        else if (blockFmt.indent() > 0)
        {
            // <li>first line<p>continuation</p></li>
            m_stream << QString(m_wrappedLineIndent, qtmw_Space);
        }
        else
        {
            fragmentText.replace(qtmw_LineBreak, qtmw_Newline);
        }
        startsOrEndsWithBacktick |=
                fragmentText.startsWith(qtmw_Backtick) || fragmentText.endsWith(qtmw_Backtick);
        QTextCharFormat fmt = frag.fragment().charFormat();
        if (fmt.isImageFormat())
        {
            QTextImageFormat ifmt = fmt.toImageFormat();
            QString desc          = ifmt.stringProperty(QTextFormat::ImageAltText);
            if (desc.isEmpty())
            {
                desc = "image"_L1;
            }
            QString s     = "!["_L1 + desc + "]("_L1 + ifmt.name();
            QString title = ifmt.stringProperty(QTextFormat::ImageTitle);
            if (!title.isEmpty())
            {
                s += (QString(qtmw_Space) + qtmw_DoubleQuote) + title + qtmw_DoubleQuote;
            }
            s += u')';
            if (inWrap && col + s.size() > ColumnLimit)
            {
                m_stream << qtmw_Newline << wrapIndentString;
                col = m_wrappedLineIndent;
            }
            m_stream << s;
            col += s.size();
        }
        else if (fmt.hasProperty(QTextFormat::AnchorHref))
        {
            const auto href       = fmt.property(QTextFormat::AnchorHref).toString();
            const bool hasToolTip = fmt.hasProperty(QTextFormat::TextToolTip);
            QString s;
            if (!hasToolTip && href == fragmentText && !QUrl(href, QUrl::StrictMode).scheme().isEmpty())
            {
                s = u'<' + href + u'>';
            }
            else
            {
                s = u'[' + fragmentText + "]("_L1 + href;
                if (hasToolTip)
                {
                    s += qtmw_Space;
                    s += createLinkTitle(fmt.property(QTextFormat::TextToolTip).toString());
                }
                s += u')';
            }
            if (inWrap && col + s.size() > ColumnLimit)
            {
                m_stream << qtmw_Newline << wrapIndentString;
                col = m_wrappedLineIndent;
            }
            m_stream << s;
            col += s.size();
        }
        else
        {
            QFontInfo fontInfo(fmt.font());
            bool monoFrag = fontInfo.fixedPitch() || fmt.fontFixedPitch();
            QString markers;
            if (!inIgnoreFormat)
            {
                if (monoFrag != mono && !m_indentedCodeBlock && !m_fencedCodeBlock)
                {
                    if (monoFrag)
                    {
                        backticks =
                                QString(adjacentBackticksCount(fragmentText) + 1, qtmw_Backtick);
                    }
                    markers += backticks;
                    if (startsOrEndsWithBacktick)
                    {
                        markers += qtmw_Space;
                    }
                    mono = monoFrag;
                    if (!mono)
                    {
                        endingMarkers = true;
                    }
                }
                if (!blockFmt.headingLevel() && !mono)
                {
                    if (fontInfo.bold() != bold)
                    {
                        markers += "**"_L1;
                        bold    = fontInfo.bold();
                        if (!bold)
                        {
                            endingMarkers = true;
                        }
                    }
                    if (fontInfo.italic() != italic)
                    {
                        markers += u'*';
                        italic  = fontInfo.italic();
                        if (!italic)
                        {
                            endingMarkers = true;
                        }
                    }
                    if (fontInfo.strikeOut() != strikeOut)
                    {
                        markers   += "~~"_L1;
                        strikeOut = fontInfo.strikeOut();
                        if (!strikeOut)
                        {
                            endingMarkers = true;
                        }
                    }
                    if (fontInfo.underline() != underline)
                    {
                        // CommonMark specifies underline as another way to get emphasis (italics):
                        // https://spec.commonmark.org/0.31.2/#example-148
                        // but md4c allows us to distinguish them; so we support underlining (in GitHub dialect).
                        markers   += u'_';
                        underline = fontInfo.underline();
                        if (!underline)
                        {
                            endingMarkers = true;
                        }
                    }
                }
            }
            if (inWrap && col + markers.size() * 2 + fragmentText.size() > ColumnLimit)
            {
                int i             = 0;
                const int fragLen = fragmentText.size();
                bool breakingLine = false;
                while (i < fragLen)
                {
                    if (col >= ColumnLimit)
                    {
                        m_stream << markers << qtmw_Newline << wrapIndentString;
                        markers.clear();
                        col = m_wrappedLineIndent;
                        while (i < fragLen && fragmentText[i].isSpace())
                        {
                            ++i;
                        }
                    }
                    int j = i + ColumnLimit - col;
                    if (j < fragLen)
                    {
                        int wi = nearestWordWrapIndex(fragmentText, j);
                        if (wi < 0)
                        {
                            j = fragLen;
                            // can't break within the fragment: we need to break already _before_ it
                            if (endingMarkers)
                            {
                                m_stream << markers;
                                markers.clear();
                            }
                            m_stream << qtmw_Newline << wrapIndentString;
                            col = m_wrappedLineIndent;
                        }
                        else if (wi >= i)
                        {
                            j            = wi;
                            breakingLine = true;
                        }
                    }
                    else
                    {
                        j            = fragLen;
                        breakingLine = false;
                    }
                    QString subfrag = fragmentText.mid(i, j - i);
                    if (!i)
                    {
                        m_stream << markers;
                        col += markers.size();
                    }
                    if (col == m_wrappedLineIndent)
                    {
                        maybeEscapeFirstChar(subfrag);
                    }
                    m_stream << subfrag;
                    if (breakingLine)
                    {
                        m_stream << qtmw_Newline << wrapIndentString;
                        col = m_wrappedLineIndent;
                    }
                    else
                    {
                        col += subfrag.size();
                    }
                    i = j + 1;
                } // loop over fragment characters (we know we need to break somewhere)
            }
            else
            {
                if (!m_linePrefixWritten && col == wrapIndentString.size())
                {
                    m_stream << m_linePrefix;
                    col += m_linePrefix.size();
                }
                m_stream << markers << fragmentText;
                col += markers.size() + fragmentText.size();
            }
        }
    }
    if (mono)
    {
        if (startsOrEndsWithBacktick)
        {
            m_stream << qtmw_Space;
            col += 1;
        }
        m_stream << backticks;
        col += backticks.size();
    }
    if (bold)
    {
        m_stream << "**";
        col += 2;
    }
    if (italic)
    {
        m_stream << "*";
        col += 1;
    }
    if (underline)
    {
        m_stream << "_";
        col += 1;
    }
    if (strikeOut)
    {
        m_stream << "~~";
        col += 2;
    }
    if (missedBlankCodeBlockLine)
    {
        m_stream << qtmw_Newline;
    }
    m_linePrefixWritten = false;
    return col;
}
