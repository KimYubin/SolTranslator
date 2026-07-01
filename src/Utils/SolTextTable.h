// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLTEXTTABLE_H
#define SOLTRANSLATOR_SOLTEXTTABLE_H
#include <QString>
#include <QTextCharFormat>

class QTextTable;


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

inline QDebug operator<<(QDebug debug, const Grid& inGrid)
{
    QString res;
    bool isFirstRow = true;
    for (const GridRow& gridRow : inGrid)
    {
        for (const CellData& curCell : gridRow)
        {
            QString curStr;
            for (const auto& [text, charFormat] : curCell.fragments)
            {
                curStr += text;
            }
            res += "|" + curStr;
        }
        res += "|\n";
        if (isFirstRow)
        {
            isFirstRow = false;
            res += QString{"|---"}.repeated(gridRow.size()) + "|\n";
        }
    }

    QDebugStateSaver saver(debug);
    debug.nospace() << res;
    return debug;
}

namespace Sol
{
Grid makeGrid(const int inRow, const int inCol, CellData inCellData);

void expendCol(Grid& inTable, const int inColPos, const int inSize);

void expendRow(Grid& inTable, const int inRowPos, const int inSize);

void expandGrid(Grid& inTable, const int inRowPos, const int inColPos, const int inRowSize, const int inColSize);

std::tuple<int, int> appendGrid(Grid& inOrigin, const Grid& inNested, const int inRowPos, const int inColPos);

/**
 * 중첩 테이블 및 다중 블록 셀을 단일 Grid 데이터로 변환합니다.
 *
 * @param inTable 변환 대상
 * @return 평면화된 테이블의 Grid 데이터
 */
Grid convertTableToGrid(QTextTable* inTable);

/**
 * Grid를 Table에 덮어 씌웁니다.
 * Table의 size는 Grid size로 변경됩니다.
 */
void gridToTable(const Grid& inGrid, QTextTable* inTable);

/**
 * Flatten the nested tables into a single table.
 */
void flattenToSingleTable(QTextTable* inTable);

void replaceNewLine(QString& inString);

} // namespace Sol 

#endif //SOLTRANSLATOR_SOLTEXTTABLE_H
