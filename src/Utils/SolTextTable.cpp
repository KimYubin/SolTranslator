// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTextTable.h"

#include <QTextTableCell>
#include <QUuid>

namespace Sol
{
const QString newLineMarker = "__NEWLINE_BR_" + QUuid::createUuid().toString(QUuid::Id128) + "_";

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
                minEmptyRow = std::min(minEmptyRow, rIdx - inRowPos);
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

void expandGrid(Grid& inTable
              , const int inRowPos
              , const int inColPos
              , const int inRowSize
              , const int inColSize)
{
    // 아래 먼저 밀기
    if (inRowSize > 1)
    {
        expendRow(inTable, inRowPos, inRowSize - 1);
    }

    if (inColSize > 1)
    {
        expendCol(inTable, inColPos, inColSize - 1);
    }
}

std::tuple<int, int> appendGrid(Grid& inOrigin
                              , const Grid& inNested
                              , const int inRowPos
                              , const int inColPos)
{
    if (inNested.empty() || inNested.front().empty())
    {
        return {0, 0};
    }

    const int targetRows = inRowPos + 1;
    const int targetCols = inColPos + 1;
    const int nestedRows = inNested.size();
    const int nestedCols = inNested.front().size();

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
                    QString newLineMarker = R"(<br/>)";
                    fragStr.replace("\r\n", newLineMarker);
                    fragStr.replace(QChar::LineFeed, newLineMarker);
                    fragStr.replace(QChar::CarriageReturn, newLineMarker);
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

void flattenToSingleTable(QTextTable* inTable)
{
    if (inTable == nullptr)
    {
        return;
    }

    //  Convert table even without nested tables.
    //  To fix issues such as line breaks within cells and block separation.
    const Grid grid = convertTableToGrid(inTable);
    gridToTable(grid, inTable);

}

void replaceNewLine(QString& inString)
{
    inString.replace(newLineMarker, R"(<br/>)");
}
} // namespace Sol 
