// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolDocument.h"

#include "SolAsync.hpp"
#include "SolChrono.h"
#include "SolLog.h"
#include "SolTextTable.h"
#include "QtCustom/SolMarkdownWriter.h"
#include "Types/SolGuard.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QTextTableCell>

namespace
{
/**
 * Fix the internal error of the list items.
 */
void fixListItem(QTextDocument& inDoc);

/**
 * Fix the last space of in Bold(**).
 * Prevent broken bold.
 */
void fixBoldLastSpace(QTextDocument& inDoc);

/**
 * Integrate the divided blocks in the cell.
 * Prevent the divided blocks from being interpreted as adjacent cells.
 */
void fixTable(QTextDocument& inDoc);

/**
 * Converts QTextDocument to Markdown. Customized QTextDocument::toMarkdown().
 */
QString toSolMarkdown(QTextDocument& inDoc);

} // anonymous namespace


namespace Sol
{
QString textDocumentToMarkdown(QTextDocument& inDoc)
{
    fixListItem(inDoc);
    fixBoldLastSpace(inDoc);
    fixTable(inDoc);

    QString markdownStr = toSolMarkdown(inDoc);

    return markdownStr;
}

QString htmlToMarkdown(QString inHtml)
{
    // Remove the syntax that ignores the list.
    inHtml.replace(QRegularExpression(R"(list-style: none)"), "");
    QTextDocument txtDoc;
    txtDoc.setHtml(inHtml);

    return textDocumentToMarkdown(txtDoc);
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
void fixListItem(QTextDocument& inDoc)
{
    QTextCursor fragCursor(&inDoc);
    TextCursorEditBlockGuard cursorEditBlockGuard{fragCursor};

    for (QTextBlock textBlock = inDoc.begin(); textBlock.isValid(); textBlock = textBlock.next())
    {
        QTextList* txtList = QTextCursor(textBlock).currentList();
        if (!txtList)
        {
            continue;
        }

        // Fix hypertext link error.
        for (QTextBlock::iterator itemIt = textBlock.begin(); !itemIt.atEnd(); ++itemIt)
        {
            QTextFragment fragment = itemIt.fragment();
            if (!fragment.isValid())
            {
                continue;
            }

            QTextCharFormat fragFmt = fragment.charFormat();
            const bool hasLinkText  = (!fragFmt.anchorHref().isEmpty());
            if (fragFmt.isAnchor() == hasLinkText)
            {
                continue;
            }

            fragFmt.setAnchor(hasLinkText);

            const int fragStart = fragment.position();
            const int frageEnd  = fragment.position() + fragment.length();

            fragCursor.setPosition(fragStart);
            fragCursor.setPosition(frageEnd, QTextCursor::KeepAnchor);

            fragCursor.mergeCharFormat(fragFmt);
        }

        // Merge the fragments that have the same Href.
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
            QTextFragment lastFrag = nextIt.fragment();
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
                lastFrag      = nextIt.fragment();

                ++nextIt;
            }

            if (isNeedMerge == false)
            {
                continue;
            }


            // Remove newline in list item.
            mergeLinkText.removeIf([](const QChar& inChar)
            {
                return (inChar == QChar::LineFeed)
                        || (inChar == QChar::CarriageReturn)
                        || (inChar == QChar::LineSeparator)
                        || (inChar == QChar::ParagraphSeparator);
            });

            // Connect the separated links.
            // The front link format is broken, so Apply the last fragment format.
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

void fixBoldLastSpace(QTextDocument& inDoc)
{
    QTextCursor textCursor(&inDoc);
    TextCursorEditBlockGuard cursorEditBlockGuard{textCursor};

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

            // Recreate the modified iterator after the update.
            const int curBlockPos = textBlock.position();
            for (textBlock = inDoc.begin(); textBlock.isValid(); textBlock = textBlock.next())
            {
                if (textBlock.position() >= curBlockPos)
                {
                    break;
                }
            }
            // for renew textBlock
            break;
        }
    }
}


// ~============================
// tables
void fixTable(QTextDocument& inDoc)
{
    QTextFrame* rootFrame  = inDoc.rootFrame();
    QTextFrame* childFrame = nullptr;

    for (QTextFrame::iterator rootIt = rootFrame->begin(); !rootIt.atEnd(); ++rootIt)
    {
        QTextFrame* curFrame = rootIt.currentFrame();
        if (curFrame && childFrame != curFrame)
        {
            if (QTextTable* table = qobject_cast<QTextTable*>(curFrame))
            {
                Sol::flattenToSingleTable(table);
            }
        }

        childFrame = curFrame;
    }
}


QString toSolMarkdown(QTextDocument& inDoc)
{
    QString res;
    QTextStream testStream(&res);
    SolMarkdownWriter mdWriter(testStream, QTextDocument::MarkdownDialectGitHub);
    if (mdWriter.writeAll(&inDoc))
    {
        res.replace(QChar::Nbsp, " ");
        Sol::replaceNewLine(res);

        return res;
    }
    return QString();
}

} // anonymous namespace

TextCursorEditBlockGuard::TextCursorEditBlockGuard(QTextCursor& inCursor)
    : SolGeneralGuard([&inCursor]() { inCursor.endEditBlock(); })
{
    inCursor.beginEditBlock();
}
