// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTextTable.h"

#include "SolLog.h"

#include <QTextTableCell>
#include <QUuid>

namespace
{
const QString newLineMarker = "__NEWLINE_BR_" + QUuid::createUuid().toString(QUuid::Id128) + "__";
const QString newLineBrTag  = R"(<br/>)";
} // anonymous namespace

namespace Sol
{
Grid makeGrid(const int inRow, const int inCol, CellData inCellData)
{
    return Grid(inRow, GridRow(inCol, std::move(inCellData)));
}

int insertCol(Grid& inGrid, const int inColPos, const int inSize)
{
    if (inSize <= 0 || inGrid.empty())
    {
        return 0;
    }

    const int originColSize = inGrid.front().size();

    if (originColSize <= inColPos)
    {
        const int emptyColSize = (inColPos + inSize) - originColSize;
        GridRow emptyCol(emptyColSize, CellData());
        for (GridRow& curRow : inGrid)
        {
            curRow.append_range(emptyCol);
        }
        return emptyColSize;
    }

    int minEmptyColCount = std::min<int>(inSize, originColSize - inColPos);
    for (GridRow& curRow : inGrid)
    {
        const int colMax = std::min<int>(curRow.size(), inColPos + minEmptyColCount);
        for (int cIdx = inColPos; cIdx < colMax; ++cIdx)
        {
            if (!curRow[cIdx].isEmpty)
            {
                minEmptyColCount = std::min(minEmptyColCount, cIdx - inColPos);
                break;
            }
        }
    }

    const int needColSize = inSize - minEmptyColCount;
    GridRow emptyCol(needColSize, CellData());
    for (GridRow& row : inGrid)
    {
        row.insert_range(row.begin() + inColPos + minEmptyColCount, emptyCol);
    }
    return needColSize;
}

int insertRow(Grid& inGrid, const int inRowPos, const int inSize)
{
    if (inSize <= 0)
    {
        return 0;
    }

    const int originRowSize = inGrid.size();
    const int originColSize = inGrid.empty() ? 0 : inGrid.front().size();

    const GridRow emptyRow(originColSize, CellData());

    if (originRowSize <= inRowPos)
    {
        const int emptyRowSize = (inRowPos + inSize) - originRowSize;
        inGrid.append_range(Grid(emptyRowSize, emptyRow));
        return emptyRowSize;
    }

    int minEmptyRowCount = std::min<int>(inSize, originRowSize - inRowPos);
    const int maxRows    = std::min(originRowSize, inRowPos + inSize);
    for (int rIdx = inRowPos; rIdx < maxRows; ++rIdx)
    {
        const GridRow& row = inGrid[rIdx];
        if (std::ranges::contains(row, false, &CellData::isEmpty))
        {
            minEmptyRowCount = std::min(minEmptyRowCount, rIdx - inRowPos);
            break;
        }
    }

    const int needRowSize = inSize - minEmptyRowCount;
    inGrid.insert_range(inGrid.begin() + inRowPos, Grid(needRowSize, emptyRow));

    return needRowSize;
}

std::tuple<int, int> insertGrid(Grid& inOrigin
                              , const int inRowPos
                              , const int inColPos
                              , Grid inNested)
{
    if (inNested.empty() || inNested.front().empty())
    {
        return {0, 0};
    }

    const int nestedRows = inNested.size();
    const int nestedCols = inNested.front().size();

    // Push Rows first.
    const int addedRows = insertRow(inOrigin, inRowPos + 1, nestedRows - 1);
    const int addedCols = insertCol(inOrigin, inColPos + 1, nestedCols - 1);

    for (int rowIdx = 0; rowIdx < nestedRows; ++rowIdx)
    {
        for (int colIdx = 0; colIdx < nestedCols; ++colIdx)
        {
            inOrigin[inRowPos + rowIdx][inColPos + colIdx] = std::move(inNested[rowIdx][colIdx]);
            inOrigin[inRowPos + rowIdx][inColPos + colIdx].isEmpty = true;
        }
    }

    return {addedRows, addedCols};
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
    const int tableRowCount = inTable->rows();
    const int tableColCount = inTable->columns();

    Grid resTable = makeGrid(tableRowCount, tableColCount, CellData(inTable));

    for (int rIdx = 0; rIdx < tableRowCount; ++rIdx)
    {
        for (int cIdx = 0; cIdx < tableColCount; ++cIdx)
        {
            // Merged cells in CellData contain duplicate data,
            // and after adding/removing columns and rows, there is a possibility of an error
            QTextTableCell curCell = inTable->cellAt(rIdx, cIdx);
            if (curCell.rowSpan() > 1 || curCell.columnSpan() > 1)
            {
                inTable->splitCell(rIdx, cIdx, 1, 1);
            }

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

            QTextTableCell curCell = curCellData.table->cellAt(curCellData.row, curCellData.col);
            for (QTextFrame::iterator cellIt = curCell.begin(); !cellIt.atEnd(); ++cellIt)
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
                    QTextFragment frag = blockIt.fragment();

                    QString fragText = frag.text();
                    fragText.replace("\r\n", newLineMarker);
                    fragText.replace(QChar::LineFeed, newLineMarker);
                    fragText.replace(QChar::CarriageReturn, newLineMarker);
                    fragText.replace(QChar::LineSeparator, newLineMarker);
                    fragText.replace(QChar::ParagraphSeparator, newLineMarker);

                    newFragments.emplace_back(std::move(fragText), frag.charFormat());
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

            auto [exRow, exCol] = insertGrid(resTable, rIdx, cIdx, std::move(NewCellGrid));
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

    const int gridRows = inGrid.size();
    const int gridCols = inGrid[0].size();

    inTable->resize(gridRows, gridCols);

    for (int rIdx = 0; rIdx < gridRows; ++rIdx)
    {
        for (int cIdx = 0; cIdx < gridCols; ++cIdx)
        {
            QTextTableCell curCell = inTable->cellAt(rIdx, cIdx);
            QTextCursor cellCursor = curCell.firstCursorPosition();
            cellCursor.setPosition(curCell.lastPosition(), QTextCursor::KeepAnchor);

            const std::vector<TextFragmentData>& curFragments = inGrid[rIdx][cIdx].fragments;
            if (curFragments.empty())
            {
                // SolMarkdownImporter supports whitespace and empty cell rendering. Qt Markdown does not.
                cellCursor.insertText("");
                continue;
            }

            for (const auto& [frgText, fragCharFormat] : curFragments)
            {
                cellCursor.insertText(frgText, fragCharFormat);
            }
        }
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
    inString.replace(newLineMarker, newLineBrTag);
}
} // namespace Sol 
