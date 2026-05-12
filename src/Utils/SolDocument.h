// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDOCUMENT_H
#define SOLTRANSLATOR_SOLDOCUMENT_H
#include <QTextDocument>

namespace Sol
{

// html 문법 정리를 위한 normalize.
void normalizeHtml(QTextDocument& inDoc);

// 강조 표시(**) 내부 마지막 공백 수정
void fixTailSpaceInBold(QTextDocument& inDoc);

// 테이블 내부 블록 나뉨 수정. 나눠진 블록은 옆 cell로 해석될 수 있음.
void fixTableCell(QTextDocument& inDoc);

// 테이블 내부 줄바꿈을 마크다운에서 깨지지 안도록 조정
QString fixNewLineInTable(QTextDocument& inDoc);

} // namespace Sol 


#endif //SOLTRANSLATOR_SOLDOCUMENT_H
