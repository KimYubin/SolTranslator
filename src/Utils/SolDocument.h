// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLDOCUMENT_H
#define SOLTRANSLATOR_SOLDOCUMENT_H

#include "Types/SolGuard.h"
#include "Types/SolTypes.h"

class QTextCursor;
class QObject;
class QTextDocument;
class QString;

namespace Sol
{

/**
 * Convert HTML string to Markdown.
 * Correct any distortion that occurs during the conversion.
 */
QString textDocumentToMarkdown(QTextDocument& inDoc);

/**
 * Convert HTML string to Markdown.
 * Correct any distortion that occurs during the conversion.
 */
QString htmlToMarkdown(QString inHtml);


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



class TextCursorEditBlockGuard : public SolGeneralGuard
{
public:
    [[nodiscard]]
    explicit TextCursorEditBlockGuard(QTextCursor& inCursor);
};


} // namespace Sol

#endif //SOLTRANSLATOR_SOLDOCUMENT_H
