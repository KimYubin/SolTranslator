// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolDocument.h"

#include "SolAsync.hpp"
#include "SolLog.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextList>
#include <QTextTableCell>
#include <QUuid>

namespace
{
const QString codeBlockMarker        = "__CODE_BLOCK_" + QUuid::createUuid().toString(QUuid::Id128) + "__";
const QString codeBlockNewLineMarker = "__CODE_NEWLINE_" + QUuid::createUuid().toString(QUuid::Id128).left(8) + "__";
} // anonymous namespace

namespace Sol
{
QString htmlToMarkdown(QString inHtml)
{
    // Remove the syntax that ignores the list.
    QTextDocument txtDoc;
    inHtml.replace(QRegularExpression(R"(list-style: none)"), "");
    txtDoc.setHtml(inHtml);
    return htmlToMarkdown(txtDoc);
}

QString htmlToMarkdown(QTextDocument& inDoc)
{
    fixListItem(inDoc);

    codeBlockToMarker(inDoc);

    normalizeHtml(inDoc);
    fixTailSpaceInBold(inDoc);
    fixTableCell(inDoc);

    QString markdownStr = fixNewLine(inDoc);

    return markerToCodeBlock(markdownStr);;
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

void codeBlockToMarker(QTextDocument& inDoc)
{
    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); /* textBlock = next; */)
    {
        QTextBlock next = textBlock.next();

        QMap<int, QVariant> properties = textBlock.blockFormat().properties();
        if (properties.contains(QTextFormat::BlockNonBreakableLines))
        {
            QTextCursor cursor(textBlock);

            // Marking the start of the code block.
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.insertText(codeBlockMarker);

            // Marking line break in code block.
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
            cursor.insertText(codeBlockMarker);
        }

        textBlock = next;
    }
}

QString& markerToCodeBlock(QString& inString)
{
    inString.replace("```\n" + codeBlockMarker, "```\n");
    inString.replace(codeBlockMarker + "\n```", "\n```");

    inString.replace("`" + codeBlockMarker, "```\n");
    inString.replace(codeBlockMarker + "`", "\n```");

    inString.replace(codeBlockNewLineMarker, "\n");
    return inString;
}

void normalizeHtml(QTextDocument& inDoc)
{
    inDoc.setHtml(inDoc.toHtml());
}

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
                    break;

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


            // Remove new line in list item.
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
        for (QTextFrame::iterator it = cell.begin(); !it.atEnd(); ++it)
        {
            // 가장 앞쪽 block으로 통합
            it = cell.begin();

            QTextBlock cellBlock = it.currentBlock();
            if (!cellBlock.isValid())
            {
                continue;
            }

            QTextCursor cellCursor(cellBlock);

            cellCursor.movePosition(QTextCursor::EndOfBlock);
            cellCursor.deleteChar();
        }
    }
}

QString fixNewLine(QTextDocument& inDoc)
{
    // ~====================
    // fix line breaks in table cell.
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

        // Replace line break in table
        QString tableHtml = match.captured();
        tableHtml.replace(brPattern, newLineMarker);
        replacedHtml += tableHtml;

        lastPos = match.capturedEnd();
    }

    replacedHtml += originHtml.mid(lastPos);


    // Convert to Markdown.
    inDoc.setHtml(replacedHtml);
    QString docMarkdown = inDoc.toMarkdown();

    docMarkdown.replace(QChar::Nbsp, " ");

    // ~====================
    // Convert single line break to a space.
    // Convert consecutive spaces before and after a single line break to a space.
    // Do not modify consecutive line breaks.
    // If a list item follows a single line break, do not modify it.
    docMarkdown.replace(
        QRegularExpression(
            R"([^\S\n]*(?<!\n)\n(?!\n)(?![^\S\n]*([-*+]|\d+\.))[^\S\n]*)"
        ), " ");

    // ~====================
    // Replace marker to <br/>.
    docMarkdown.replace(newLineMarker, R"(<br/>)");

    return docMarkdown;
}
} // namespace Sol 
