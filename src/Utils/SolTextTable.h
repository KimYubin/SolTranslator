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
    debug.nospace();
    debug.noquote() << "\n" << res;

    return debug;
}

namespace Sol
{
Grid makeGrid(const int inRow, const int inCol, CellData inCellData);

/**
 * inColPos부터 inSize 만큼 범위 내에 빈 셀을 삽입합니다.
 * 범위 내에 빈셀로만 이루어진 column이 있다면 그만큼 덜 삽입합니다.
 * inColPos가 inGrid 범위를 밖이라면 자동으로 확장합니다.
 *
 * @return 실제로 삽입한 column의 개수.
 */
int insertCol(Grid& inGrid, const int inColPos, const int inSize);

int insertRow(Grid& inGrid, const int inRowPos, const int inSize);

/**
 * \a inOrigin 의 inRowPos, inColPos 셀에 inNested를 삽입합니다.
 *
 * @return 
 */
std::tuple<int, int> insertGrid(Grid& inOrigin, const int inRowPos, const int inColPos, Grid inNested);

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
