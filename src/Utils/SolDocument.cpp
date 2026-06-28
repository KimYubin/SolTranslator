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

void fixTable(QTextDocument& inDoc);

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

void flattenToSingleTable(QTextTable* inTable);
} // anonymous namespace


namespace Sol
{
QString htmlToMarkdown(QTextDocument& inDoc)
{
    fixListItem(inDoc);

    codeBlockToMarker(inDoc);

    normalizeHtml(inDoc);

    fixTailSpaceInBold(inDoc);
    fixTable(inDoc);

    QString markdownStr = fixNewLine(inDoc);
    markerToCodeBlock(markdownStr);

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
                flattenToSingleTable(table);
            }
        }

        childFrame = curFrame;
    }

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


// tables
struct TextFragmentData
{
    QString text;
    QTextCharFormat charFormat;
};

struct CellData
{
    CellData() = default;

    explicit CellData(const bool inIsEmpty)
        : isEmpty(inIsEmpty)
    {}

    explicit CellData(QTextTable* inTable)
        : isEmpty(false)
        , table(inTable)
    {}

    explicit CellData(std::vector<TextFragmentData> inFragments)
        : isEmpty(false)
        , fragments(std::move(inFragments))
    {}

    bool isEmpty = true;

    QTextTable* table = nullptr;
    int row = -1;
    int col = -1;
    std::vector<TextFragmentData> fragments;
};


using GridRow = std::vector<CellData>;
using Grid    = std::vector<GridRow>;


Grid makeGrid(const int inRow, const int inCol, CellData inCellData)
{
    return Grid(inRow, GridRow(inCol, std::move(inCellData)));
}

void printGrid(const Grid& inGrid)
{
    QString res;
    for (int rIdx = 0; rIdx < inGrid.size(); ++rIdx)
    {
        for (const CellData& cur : inGrid[rIdx])
        {
            QString curStr;
            for (auto& frag : cur.fragments)
            {
                curStr += frag.text;
            }
            res += "|" + curStr;
        }
        res += "|\n";
        if (rIdx == 0)
        {
            res += QString{"|---"}.repeated(inGrid[rIdx].size()) + "|\n";
        }
    }
    solDebug << res;
}

void expendCol(Grid& inTable, const int posCol, const int insertCol)
{
    const int originColSize = inTable.front().size();

    if (originColSize <= posCol)
    {
        const int emptyColSize = (posCol + insertCol) - originColSize;
        GridRow emptyCol(emptyColSize, CellData());
        for (GridRow& curRow : inTable)
        {
            curRow.append_range(emptyCol);
        }
        return;
    }

    int minEmptyCol = insertCol;
    for (GridRow& curRow : inTable)
    {
        const int colMax = std::min<int>(curRow.size(), posCol + minEmptyCol);
        for (int cIdx = posCol; cIdx < colMax; ++cIdx)
        {
            if (!curRow[cIdx].isEmpty)
            {
                minEmptyCol = std::min(minEmptyCol, cIdx - posCol);
                break;
            }
        }
    }

    GridRow emptyCol(insertCol - minEmptyCol, CellData());
    for (GridRow& row : inTable)
    {
        row.insert_range(row.begin() + posCol + minEmptyCol, emptyCol);
    }
}

void expendRow(Grid& inTable, const int posRow, const int insertRow)
{
    const int originRowSize = inTable.size();
    const int originColSize = inTable.front().size();

    const GridRow emptyRow(originColSize, CellData());

    if (originRowSize <= posRow)
    {
        const int emptyRowSize = (posRow + insertRow) - originRowSize;
        inTable.append_range(Grid(emptyRowSize, emptyRow));
        return;
    }

    int minEmptyRow = insertRow;
    for (int rIdx = posRow + 1; rIdx < posRow + insertRow; ++rIdx)
    {
        const GridRow& row = inTable[rIdx];
        for (int cIdx = 0; cIdx < row.size(); ++cIdx)
        {
            if (!row[cIdx].isEmpty)
            {
                minEmptyRow = std::min(minEmptyRow, rIdx - (posRow + 1));
                break;
            }
        }
        if (minEmptyRow != insertRow)
        {
            break;
        }
    }

    inTable.insert_range(inTable.begin() + posRow, Grid(insertRow - minEmptyRow, emptyRow));
}

void expandGrid(Grid& inTable, const int posRow, const int posCol, const int insertRow, const int insertCol)
{
    // 아래 먼저 밀기
    if (insertRow > 1)
    {
        expendRow(inTable, posRow, insertRow);
    }

    if (insertCol > 1)
    {
        expendCol(inTable, posCol, insertCol);
    }
}

std::tuple<int, int> appendGrid(Grid& inOrigin, const Grid& inNested, const int oRowIdx, const int oColIdx)
{
    if (inNested.empty() || inNested.front().empty())
    {
        return {0, 0};
    }

    const int nestedRows = inNested.size();
    const int nestedCols = inNested.front().size();
    const int targetRows = oRowIdx + 1;
    const int targetCols = oColIdx + 1;

    expandGrid(inOrigin, targetRows, targetCols, nestedRows, nestedCols);

    for (int rowIdx = 0; rowIdx < nestedRows; ++rowIdx)
    {
        for (int colIdx = 0; colIdx < nestedCols; ++colIdx)
        {
            inOrigin[oRowIdx + rowIdx][oColIdx + colIdx]         = inNested[rowIdx][colIdx];
            inOrigin[oRowIdx + rowIdx][oColIdx + colIdx].isEmpty = true;
        }
    }

    return {nestedRows, nestedCols};
}

/**
 * 중첩 테이블 및 다중 블록 셀을 단일 Grid 데이터로 변환합니다.
 *
 * @param inTable 변환 대상
 * @return 평면화된 테이블의 Grid 데이터
 */
Grid convertTableToGrid(QTextTable* inTable)
{
    if (inTable == nullptr)
    {
        return {};
    }

    Grid resTable = makeGrid(inTable->rows(), inTable->columns(), CellData(inTable));
    for (int rIdx = 0; rIdx < inTable->rows(); ++rIdx)
    {
        for (int cIdx = 0; cIdx < inTable->columns(); ++cIdx)
        {
            resTable[rIdx][cIdx].row = rIdx;
            resTable[rIdx][cIdx].col = cIdx;
        }
    }

    for (int rIdx = 0; rIdx < resTable.size(); ++rIdx)
    {
        for (int cIdx = 0; cIdx < resTable.front().size(); ++cIdx)
        {
            const CellData curCellData = resTable[rIdx][cIdx];
            if (curCellData.isEmpty)
            {
                continue;
            }

            std::vector<Grid> cellGridList;

            QTextTableCell cell = curCellData.table->cellAt(curCellData.row, curCellData.col);
            for (QTextFrame::iterator cellIt = cell.begin(); !cellIt.atEnd(); ++cellIt)
            {
                if (QTextTable* innerTable = qobject_cast<QTextTable*>(cellIt.currentFrame()))
                {
                    Grid innerGrid = convertTableToGrid(innerTable);
                    cellGridList.push_back(std::move(innerGrid));
                    continue;
                }

                // Split the internal blocks of the cell into individual cells.
                QTextBlock block = cellIt.currentBlock();
                if (block.isValid() == false || block.length() <= 0)
                {
                    continue;
                }

                std::vector<TextFragmentData> newFragments;
                for (QTextBlock::iterator blockIt = block.begin(); !blockIt.atEnd(); ++blockIt)
                {
                    QTextFragment frags = blockIt.fragment();
                    newFragments.emplace_back(frags.text(), frags.charFormat());
                }

                if (newFragments.empty())
                {
                    continue;
                }

                CellData newCellData  = curCellData;
                newCellData.fragments = std::move(newFragments);
                cellGridList.push_back(makeGrid(1, 1, std::move(newCellData)));
            }

            int maxWidth    = 0;
            int totalHeight = 0;
            for (const Grid& cellGrid : cellGridList)
            {
                if (cellGrid.empty())
                {
                    continue;
                }

                maxWidth    = std::max<int>(maxWidth, cellGrid.front().size());
                totalHeight += cellGrid.size();
            }

            Grid NewCellGrid;
            NewCellGrid.reserve(totalHeight);
            for (Grid& cellGrid : cellGridList)
            {
                for (GridRow& cellCol : cellGrid)
                {
                    cellCol.resize(maxWidth);
                }
                NewCellGrid.append_range(std::move(cellGrid));
            }

            if (NewCellGrid.empty() || NewCellGrid.front().empty())
            {
                continue;
            }

            auto [exRow, exCol] = appendGrid(resTable, NewCellGrid, rIdx, cIdx);
            cIdx                += std::max((exCol - 1), 0);
        }
    }

    return resTable;
}

/**
 * Grid를 Table에 덮어 씌웁니다.
 * Table의 size는 Grid size로 변경됩니다.
 */
void gridToTable(const Grid& inGrid, QTextTable* inTable)
{
    if (inGrid.empty())
    {
        return;
    }

    const int tableRowCount = inTable->rows();
    const int tableColCount = inTable->columns();

    const int gridRows = inGrid.size();
    const int gridCols = inGrid[0].size();

    if (tableColCount < gridCols)
    {
        inTable->appendColumns(gridCols - tableColCount);
    }
    inTable->appendRows(gridRows);

    for (int rIdx = 0; rIdx < gridRows; ++rIdx)
    {
        for (int cIdx = 0; cIdx < gridCols; ++cIdx)
        {
            QTextTableCell curCell = inTable->cellAt(tableRowCount + rIdx, cIdx);
            QTextCursor cellCursor = curCell.firstCursorPosition();
            if (inGrid[rIdx][cIdx].fragments.empty())
            {
                // Qt markdown에서 빈 셀은 렌더링 되지 않을 수 있기 때문에, 공백을 추가합니다.
                cellCursor.insertText(" ");
                continue;
            }

            for (const auto& [frgText, fragCharFormat] : inGrid[rIdx][cIdx].fragments)
            {
                cellCursor.insertText(frgText, fragCharFormat);
            }
        }
    }

    inTable->removeRows(0, tableRowCount);
    if (tableColCount > gridCols)
    {
        inTable->removeColumns(gridCols, tableColCount - gridCols);
    }
}

void flattenToSingleTable(QTextTable* inTable)
{
    if (inTable == nullptr)
    {
        return;
    }
    const Grid grid = convertTableToGrid(inTable);
    // printGrid(grid);
    gridToTable(grid, inTable);
}
} // anonymous namespace
