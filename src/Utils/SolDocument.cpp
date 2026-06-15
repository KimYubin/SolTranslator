// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolDocument.h"

#include "SolAsync.hpp"
#include "SolLog.h"
#include "SolMarkdownWriter.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QTextTableCell>
#include <QUuid>

namespace
{

void fixListItem(QTextDocument& inDoc);

void codeBlockToMarker(QTextDocument& inDoc);

QString& markerToCodeBlock(QString& inString);


void normalizeHtml(QTextDocument& inDoc);

void fixTailSpaceInBold(QTextDocument& inDoc);

void fixTableCell(QTextDocument& inDoc);

QString fixNewLine(QTextDocument& inDoc);

QString toMarkdown(QTextDocument& inDoc)
{
    QString ret;
    QTextStream s(&ret);
    MarkdownWriter w(s, QTextDocument::MarkdownDialectGitHub);
    if (w.writeAll(&inDoc))
    {
        return ret;
    }
    return QString();
}
} // anonymous namespace


namespace Sol
{
QString htmlToMarkdown(QTextDocument& inDoc)
{
    fixListItem(inDoc);

    codeBlockToMarker(inDoc);

    normalizeHtml(inDoc);
    fixTailSpaceInBold(inDoc);
    // fixTableCell(inDoc);

    QString markdownStr = fixNewLine(inDoc);
    markerToCodeBlock(markdownStr);
    // markdownStr = toMarkdown(inDoc);

    return markdownStr;
}

QString htmlToMarkdown(QString inHtml)
{
    // Remove the syntax that ignores the list.
    QTextDocument txtDoc;
    inHtml.replace(QRegularExpression(R"(list-style: none)"), "");
    txtDoc.setHtml(inHtml);
    return htmlToMarkdown(txtDoc);
}

void asyncHtmlToMarkdown(QString inHtml
                       , QObject* inContext
                       , Callback<void(const QString&)>&& inMainThreadFunc)
{
    SolAsync::asyncLaunch<QString>(
        inContext,
        [htmlStr = std::move(inHtml)]() mutable
        {
            return htmlToMarkdown(std::move(htmlStr));
        },
        std::move(inMainThreadFunc)
    );
}
} // namespace Sol


namespace
{
const QString codeFrontBlockMarker   = "__CODE_F_BLK_" + QUuid::createUuid().toString(QUuid::Id128).left(8) + "__";
const QString codeBackBlockMarker    = "__CODE_B_BLK_" + QUuid::createUuid().toString(QUuid::Id128).left(8) + "__";
const QString codeBlockNewLineMarker = "__CODE_LF_" + QUuid::createUuid().toString(QUuid::Id128).left(8) + "__";


/**
 * Fix the internal error of the list items.
 */
void fixListItem(QTextDocument& inDoc)
{
    QTextCursor fragCursor(&inDoc);

    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); textBlock = textBlock.next())
    {
        QTextCursor cursor(textBlock);
        QTextList* txtList = cursor.currentList();
        if (!txtList)
        {
            continue;
        }

        // fix hyper link error.
        for (QTextBlock::iterator itemIt = textBlock.begin(); !itemIt.atEnd(); ++itemIt)
        {
            QTextFragment fragment = itemIt.fragment();
            if (!fragment.isValid())
            {
                continue;
            }

            QTextCharFormat fragFmt = fragment.charFormat();
            if (fragFmt.isAnchor() && fragFmt.anchorHref().isEmpty())
            {
                fragFmt.setAnchor(false);
            }
            else if (!fragFmt.isAnchor() && !fragFmt.anchorHref().isEmpty())
            {
                fragFmt.setAnchor(true);
            }
            else
            {
                continue;
            }

            const int fragStart = fragment.position();
            const int frageEnd  = fragment.position() + fragment.length();

            fragCursor.setPosition(fragStart);
            fragCursor.setPosition(frageEnd, QTextCursor::KeepAnchor);

            fragCursor.mergeCharFormat(fragFmt);
        }

        // Connect the links that have the same Href.
        for (QTextBlock::iterator itemIt = textBlock.begin(); !itemIt.atEnd(); ++itemIt)
        {
            QTextFragment fragment = itemIt.fragment();
            if (!fragment.isValid())
            {
                continue;
            }

            QTextCharFormat firstFragFmt = fragment.charFormat();
            if (!firstFragFmt.isAnchor() && firstFragFmt.anchorHref().isEmpty())
            {
                continue;
            }

            const int frgStart    = fragment.position();
            bool isNeedMerge      = false;
            QString mergeLinkText = fragment.text();

            const QString firstHref = firstFragFmt.anchorHref();

            // Track fragments with the same Href.
            QTextBlock::iterator nextIt = itemIt;
            ++nextIt;
            QTextBlock::iterator lastIt = nextIt;
            while (!nextIt.atEnd())
            {
                QTextFragment nextFrag = nextIt.fragment();
                if (!nextFrag.isValid())
                {
                    break;
                }

                QTextCharFormat nextFmt = nextFrag.charFormat();
                if (!nextFmt.isAnchor() || nextFmt.anchorHref() != firstHref)
                {
                    break;
                }

                isNeedMerge   = true;
                mergeLinkText += nextFrag.text();
                lastIt        = nextIt;

                ++nextIt;
            }

            if (isNeedMerge == false)
            {
                continue;
            }


            // Remove newline in list item.
            mergeLinkText.removeIf([](const QChar& inChar)
            {
                return (inChar == '\n')
                        || (inChar == QChar::LineSeparator)
                        || (inChar == QChar::ParagraphSeparator)
                        || (inChar == QChar::CarriageReturn);
            });

            QTextFragment lastFrag = lastIt.fragment();

            // Connect the separated links.
            fragCursor.setPosition(frgStart);
            fragCursor.setPosition(lastFrag.position() + lastFrag.length(), QTextCursor::KeepAnchor);
            fragCursor.insertText(mergeLinkText, lastFrag.charFormat());

            // Recreate the modified iterator after the update.
            itemIt = textBlock.begin();
            while (!itemIt.atEnd())
            {
                QTextFragment frg = itemIt.fragment();
                if (frg.isValid() && frg.position() >= frgStart)
                {
                    break;
                }

                ++itemIt;
            }
        }
    }
}

/**
 * Replace the beginning, end, and line breaks of the code block, to Marker.
 * Fix the error of code blocks being changed to inline code during the HTML
 * modification process.
 *
 * @see markerToCodeBlock()
 */
void codeBlockToMarker(QTextDocument& inDoc)
{
    QTextCharFormat blockFmt;

    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); /* textBlock = next; */)
    {
        QTextBlock next = textBlock.next();

        QMap<int, QVariant> properties = textBlock.blockFormat().properties();
        if (properties.contains(QTextFormat::BlockNonBreakableLines))
        {
            QTextCursor cursor(textBlock);

            // Marking the start of the code block.
            cursor.movePosition(QTextCursor::StartOfBlock);
            QTextDocumentFragment frontFrg = QTextDocumentFragment::fromPlainText(codeFrontBlockMarker);
            cursor.setCharFormat(QTextCharFormat());
            cursor.insertFragment(frontFrg);

            // Marking newline in code block.
            for (QTextBlock::iterator itemIt = textBlock.begin(); !itemIt.atEnd(); ++itemIt)
            {
                QTextFragment frag = itemIt.fragment();

                if (!frag.isValid())
                {
                    continue;
                }

                const int frgStart = frag.position();

                QString text = frag.text();
                text.replace(QChar::LineSeparator, codeBlockNewLineMarker);

                QTextCursor fragCursor(&inDoc);
                fragCursor.setPosition(frgStart);
                fragCursor.setPosition(frgStart + frag.length(), QTextCursor::KeepAnchor);
                fragCursor.insertText(text, frag.charFormat());

                // Recreate the modified iterator after the update.
                itemIt = textBlock.begin();
                while (!itemIt.atEnd())
                {
                    QTextFragment curFrg = itemIt.fragment();
                    if (curFrg.isValid() && curFrg.position() >= frgStart)
                    {
                        break;
                    }

                    ++itemIt;
                }
            }

            // Marking the end of the code block.
            cursor.movePosition(QTextCursor::EndOfBlock);
            QTextDocumentFragment backFrg = QTextDocumentFragment::fromPlainText(codeBackBlockMarker);
            cursor.setCharFormat(QTextCharFormat());
            cursor.insertFragment(backFrg);
        }

        textBlock = next;
    }
}

/**
 * Replace the marker to origin code block.
 *
 * @see codeBlockToMarker()
 */
QString& markerToCodeBlock(QString& inString)
{
    const static QString frontBacktick = "```\n";
    const static QString backBacktick  = "\n```";

    inString.replace(frontBacktick + codeFrontBlockMarker, frontBacktick);
    inString.replace(codeBackBlockMarker + backBacktick, backBacktick);

    static const QString reStr{"`*%1`*"};
    const QRegularExpression frontRe(reStr.arg(QRegularExpression::escape(codeFrontBlockMarker)));
    const QRegularExpression backRe(reStr.arg(QRegularExpression::escape(codeBackBlockMarker)));

    inString.replace(frontRe, frontBacktick);
    inString.replace(backRe, backBacktick);

    inString.replace("`" + codeFrontBlockMarker, frontBacktick);
    inString.replace(codeBackBlockMarker + "`", backBacktick);

    inString.replace(codeBlockNewLineMarker, "\n");
    return inString;
}

/**
 * Normalize the HTML in the QTextDocument to Qt HTML style.
 */
void normalizeHtml(QTextDocument& inDoc)
{
    const QString html = inDoc.toHtml();
    inDoc.setHtml(html);
}

/**
 * Fix the last space of in Bold(**).
 * Prevent broken bold.
 */
void fixTailSpaceInBold(QTextDocument& inDoc)
{
    QTextCursor textCursor(&inDoc);

    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); textBlock = textBlock.next())
    {
        for (QTextBlock::iterator blockIt = textBlock.begin(); !blockIt.atEnd(); ++blockIt)
        {
            QTextFragment fragment = blockIt.fragment();
            if (!fragment.isValid())
            {
                continue;
            }

            QTextCharFormat charFmt = fragment.charFormat();
            if (charFmt.fontWeight() < QFont::DemiBold)
            {
                continue;
            }

            // Check if the last character is a space.
            const QString fragText = fragment.text();
            if (fragText.isEmpty() || (!fragText.back().isSpace()))
            {
                continue;
            }

            // Find the last index that is not a space.
            int tailSpaceStartIdx = 0;
            for (int idx = fragText.size() - 1; idx >= 0; --idx)
            {
                if (fragText[idx].isSpace() == false)
                {
                    tailSpaceStartIdx = idx + 1;
                    break;
                }
            }

            if (tailSpaceStartIdx == fragText.size())
            {
                continue;
            }

            const int tailSpaceStart = fragment.position() + tailSpaceStartIdx;
            const int tailSpaceEnd   = fragment.position() + fragText.size();

            textCursor.setPosition(tailSpaceStart);
            textCursor.setPosition(tailSpaceEnd, QTextCursor::KeepAnchor);

            QTextCharFormat normalFmt = charFmt;
            normalFmt.setFontWeight(QFont::Normal);

            textCursor.mergeCharFormat(normalFmt);
        }
    }
}

/**
 * Integrate the divided blocks in the cell.
 * Prevent the divided blocks from being interpreted as adjacent cells.
 */
void fixTableCell(QTextDocument& inDoc)
{
    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); textBlock = textBlock.next())
    {
        QTextCursor cursor(textBlock);

        // 현재 block이 table 내부인지
        QTextTable* table = cursor.currentTable();
        if (!table)
        {
            continue;
        }

        // cell 내부 block 순회
        QTextTableCell cell = table->cellAt(cursor);


        QTextCursor beginCursor(cell.begin().currentBlock());
        beginCursor.movePosition(QTextCursor::EndOfBlock);

        for (QTextFrame::iterator it = (++cell.begin()); !it.atEnd(); ++it)
        {
            QTextBlock nextBlock = it.currentBlock();
            if (!nextBlock.isValid())
            {
                // break;
            }


            for (QTextBlock::iterator itemIt = nextBlock.begin(); !itemIt.atEnd(); ++itemIt)
            {
                QTextFragment nextFrag = itemIt.fragment();
                solDebug << nextFrag.text();
                beginCursor.insertText(nextFrag.text().removeIf([](const QChar& inChar)
                {
                    return (inChar == '\n')
                            || (inChar == QChar::LineSeparator)
                            || (inChar == QChar::ParagraphSeparator)
                            || (inChar == QChar::CarriageReturn);
                }), nextFrag.charFormat());
            }
            QTextCursor curCursor(it.currentBlock());
        }
        for (QTextFrame::iterator it = (--cell.end()); it != cell.begin(); --it)
        {
            QTextBlock nextBlock = it.currentBlock();
            if (!nextBlock.isValid())
            {
                // break;
            }
            QTextCursor nextCursor(nextBlock);
            nextCursor.movePosition(QTextCursor::StartOfBlock);
            nextCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            nextCursor.removeSelectedText();
        }
    }
}

/**
 * Fix the table cell in Markdown to prevent line breaks from being broken.
 * Remove forced line breaks caused by word-wrap.
 *
 * @note If apply the return value back to QTextDocument, it may need to be fixed again.
 * @return Markdown string.
 */
QString fixNewLine(QTextDocument& inDoc)
{
    // ~====================
    // fix newlines in table cell.
    // html-> replace <br/> to marker -> convert markdown -> replace marker to <br/>.

    const QString originHtml = inDoc.toHtml();

    QString replacedHtml;
    replacedHtml.reserve(originHtml.size());

    // Only in <table>
    static const QRegularExpression tablePattern(R"(<table\b[^>]*>[\s\S]*?</table>)"
                                               , QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression brPattern(R"(<br\s*/?>)"
                                            , QRegularExpression::CaseInsensitiveOption);

    static const QString newLineMarker = "__CODE_BR_" + QUuid::createUuid().toString(QUuid::Id128) + "_";

    // Replace <br/> to marker
    QRegularExpressionMatchIterator reIt = tablePattern.globalMatch(originHtml);

    int lastPos = 0;
    while (reIt.hasNext())
    {
        QRegularExpressionMatch match = reIt.next();

        // Append text before table.
        replacedHtml += originHtml.mid(lastPos, match.capturedStart() - lastPos);

        // Replace newline in table
        QString tableHtml = match.captured();
        tableHtml.replace(brPattern, newLineMarker);
        replacedHtml += tableHtml;

        lastPos = match.capturedEnd();
    }

    replacedHtml += originHtml.mid(lastPos);


    // Convert to Markdown.
    inDoc.setHtml(replacedHtml);
    QString docMarkdown = toMarkdown(inDoc);

    docMarkdown.replace(QChar::Nbsp, " ");

    // ~====================
    // Convert single newline to a space.
    // Convert consecutive spaces before and after a single newline to a space.
    // Do not modify consecutive newlines.
    // If a list item follows a single newline, do not modify it.
    docMarkdown.replace(
        QRegularExpression(
            R"([^\S\n]*(?<!\|)(?<!\n)\n(?!\n)(?![^\S\n]*([-*+]|\d+\.))[^\S\n]*)"
        ), " ");

    // ~====================
    // Replace marker to <br/>.
    docMarkdown.replace(newLineMarker, R"(<br/>)");

    return docMarkdown;
}

} // anonymous namespace
