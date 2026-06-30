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
struct TextFragmentData
{
    QString text;
    QTextCharFormat charFormat;
};

const QString newLineMarker = "__NEWLINE_BR_" + QUuid::createUuid().toString(QUuid::Id128) + "_";

void fixListItem(QTextDocument& inDoc);

void fixTailSpaceInBold(QTextDocument& inDoc);

void fixTable(QTextDocument& inDoc);

QString docToMarkdown(QTextDocument& inDoc)
{
    QString res;
    QTextStream testStream(&res);
    SolMarkdownWriter mdWriter(testStream, QTextDocument::MarkdownDialectGitHub);
    if (mdWriter.writeAll(&inDoc))
    {
        res.replace(QChar::Nbsp, " ");
        res.replace(newLineMarker, R"(<br/>)");

        return res;
    }
    return QString();
}

} // anonymous namespace


namespace Sol
{
QString htmlToMarkdown(QTextDocument& inDoc)
{
    fixListItem(inDoc);

    fixTailSpaceInBold(inDoc);
    fixTable(inDoc);

    QString markdownStr = docToMarkdown(inDoc);

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
                return (inChar == '\n')
                        || (inChar == QChar::LineSeparator)
                        || (inChar == QChar::ParagraphSeparator)
                        || (inChar == QChar::CarriageReturn);
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

void flattenToSingleTable(QTextTable* inTable);

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
}

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

void expendCol(Grid& inTable, const int inColPos, const int inSize)
{
    const int originColSize = inTable.front().size();

    if (originColSize <= inColPos)
    {
        const int emptyColSize = (inColPos + inSize) - originColSize;
        GridRow emptyCol(emptyColSize, CellData());
        for (GridRow& curRow : inTable)
        {
            curRow.append_range(emptyCol);
        }
        return;
    }

    int minEmptyCol = inSize;
    for (GridRow& curRow : inTable)
    {
        const int colMax = std::min<int>(curRow.size(), inColPos + minEmptyCol);
        for (int cIdx = inColPos; cIdx < colMax; ++cIdx)
        {
            if (!curRow[cIdx].isEmpty)
            {
                minEmptyCol = std::min(minEmptyCol, cIdx - inColPos);
                break;
            }
        }
    }

    GridRow emptyCol(inSize - minEmptyCol, CellData());
    for (GridRow& row : inTable)
    {
        row.insert_range(row.begin() + inColPos + minEmptyCol, emptyCol);
    }
}

void expendRow(Grid& inTable, const int inRowPos, const int inSize)
{
    const int originRowSize = inTable.size();
    const int originColSize = inTable.front().size();

    const GridRow emptyRow(originColSize, CellData());

    if (originRowSize <= inRowPos)
    {
        const int emptyRowSize = (inRowPos + inSize) - originRowSize;
        inTable.append_range(Grid(emptyRowSize, emptyRow));
        return;
    }

    int minEmptyRow   = inSize;
    const int maxRows = std::min(originRowSize, inRowPos + inSize);
    for (int rIdx = inRowPos; rIdx < maxRows; ++rIdx)
    {
        const GridRow& row = inTable[rIdx];
        for (int cIdx = 0; cIdx < row.size(); ++cIdx)
        {
            if (!row[cIdx].isEmpty)
            {
                minEmptyRow = std::min(minEmptyRow, rIdx - (inRowPos + 1));
                break;
            }
        }
        if (minEmptyRow != inSize)
        {
            break;
        }
    }

    inTable.insert_range(inTable.begin() + inRowPos, Grid(inSize - minEmptyRow, emptyRow));
}

void expandGrid(Grid& inTable, const int inRowPos, const int inColPos, const int inRowSize, const int inColSize)
{
    // 아래 먼저 밀기
    if (inRowSize > 1)
    {
        expendRow(inTable, inRowPos, inRowSize);
    }

    if (inColSize > 1)
    {
        expendCol(inTable, inColPos, inColSize);
    }
}

std::tuple<int, int> appendGrid(Grid& inOrigin, const Grid& inNested, const int inRowPos, const int inColPos)
{
    if (inNested.empty() || inNested.front().empty())
    {
        return {0, 0};
    }

    const int nestedRows = inNested.size();
    const int nestedCols = inNested.front().size();
    const int targetRows = inRowPos + 1;
    const int targetCols = inColPos + 1;

    expandGrid(inOrigin, targetRows, targetCols, nestedRows, nestedCols);

    for (int rowIdx = 0; rowIdx < nestedRows; ++rowIdx)
    {
        for (int colIdx = 0; colIdx < nestedCols; ++colIdx)
        {
            inOrigin[inRowPos + rowIdx][inColPos + colIdx]         = inNested[rowIdx][colIdx];
            inOrigin[inRowPos + rowIdx][inColPos + colIdx].isEmpty = true;
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

                    QString fragStr = frags.text();

                    fragStr.replace("\r\n", newLineMarker);
                    fragStr.replace(QChar::CarriageReturn, newLineMarker);
                    fragStr.replace(QChar::LineFeed, newLineMarker);
                    fragStr.replace(QChar::LineSeparator, newLineMarker);
                    fragStr.replace(QChar::ParagraphSeparator, newLineMarker);

                    newFragments.emplace_back(std::move(fragStr), frags.charFormat());
                }

                if (newFragments.empty())
                {
                    continue;
                }

                if (!cellGridList.empty())
                {
                    // If the previous block is not a table (1x1),
                    // the cell will not be separated.
                    Grid& prevGrid = cellGridList.back();
                    if (prevGrid.size() == 1
                        && prevGrid[0].size() == 1
                        && prevGrid[0][0].table == inTable)
                    {
                        prevGrid[0][0].fragments.append_range(std::move(newFragments));
                        continue;
                    }
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
                // In Qt markdown, empty cells may not render, so add whitespace.
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

void printGrid(const Grid& inGrid)
{
    QString res;
    for (int rIdx = 0; rIdx < inGrid.size(); ++rIdx)
    {
        for (const CellData& cur : inGrid[rIdx])
        {
            QString curStr;
            for (const auto& [text, charFormat] : cur.fragments)
            {
                curStr += text;
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
