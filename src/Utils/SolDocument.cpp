// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolDocument.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextTableCell>
#include <QUuid>

namespace Sol
{
void normalizeHtml(QTextDocument& inDoc)
{
    inDoc.setHtml(inDoc.toHtml());
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

namespace
{
const QString newLineMarker = "__CODE_BR_" + QUuid::createUuid().toString(QUuid::Id128) + "_";

QString replaceNewLineInTable(const QString& inHtml)
{
    QString resStr;
    resStr.reserve(inHtml.size());

    // <table>
    static const QRegularExpression tablePattern(R"(<table\b[^>]*>[\s\S]*?</table>)"
                                               , QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression brPattern(R"(<br\s*/?>)"
                                            , QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator reIt = tablePattern.globalMatch(inHtml);

    int lastPos = 0;
    while (reIt.hasNext())
    {
        QRegularExpressionMatch match = reIt.next();

        // 표 이전의 텍스트 추가
        resStr += inHtml.mid(lastPos, match.capturedStart() - lastPos);

        // 표 내부의 줄바꿈 치환
        QString tableHtml = match.captured();
        tableHtml.replace(brPattern, newLineMarker);
        resStr += tableHtml;

        lastPos = match.capturedEnd();
    }

    resStr += inHtml.mid(lastPos);

    return resStr;
}
} // anonymous namespace


QString fixNewLineInTable(QTextDocument& inDoc)
{
    inDoc.setHtml(replaceNewLineInTable(inDoc.toHtml()));

    QString docMarkdown = inDoc.toMarkdown();

    docMarkdown.replace(newLineMarker, R"(<br/>)");

    return docMarkdown;
}
} // namespace Sol 
