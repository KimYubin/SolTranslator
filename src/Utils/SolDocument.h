// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDOCUMENT_H
#define SOLTRANSLATOR_SOLDOCUMENT_H

#include "Types/SolTypes.h"

class QObject;
class QTextDocument;
class QString;

namespace Sol
{
/**
 * Convert HTML string to Markdown.
 * Correct any distortion that occurs during the conversion.
 */
QString htmlToMarkdown(QString inHtml);

/**
 * Convert HTML string to Markdown.
 * Correct any distortion that occurs during the conversion.
 */
QString htmlToMarkdown(QTextDocument& inDoc);

/**
 * Asynchronously, Convert HTML string to Markdown.
 * Correct any distortion that occurs during the conversion.
 *
 * @param inHtml Source HTML string.
 * @param inContext QObject that manages the lifetime of the callback
 * @param inMainThreadFunc Callback to be applied after completing the Markdown change.
 */
void asyncHtmlToMarkdown(QString inHtml
                       , QObject* inContext
                       , Callback<void(const QString&)>&& inMainThreadFunc);


/**
 * Normalize the HTML in the QTextDocument to Qt HTML style.
 */
void normalizeHtml(QTextDocument& inDoc);

/**
 * Fix the internal error of the list items.
 */
void fixListItem(QTextDocument& inDoc);

/**
 * Fix the last space of in Bold(**).
 * Prevent broken bold.
 */
void fixTailSpaceInBold(QTextDocument& inDoc);

/**
 * Integrate the divided blocks in the cell.
 * Prevent the divided blocks from being interpreted as adjacent cells.
 */
void fixTableCell(QTextDocument& inDoc);

/**
 * Fix the table cell in Markdown to prevent line breaks from being broken.
 * Remove forced line breaks caused by word-wrap.
 *
 * @note If apply the return value back to QTextDocument, it may need to be fixed again.
 * @return Markdown string.
 */
QString fixNewLine(QTextDocument& inDoc);
} // namespace Sol 


#endif //SOLTRANSLATOR_SOLDOCUMENT_H
