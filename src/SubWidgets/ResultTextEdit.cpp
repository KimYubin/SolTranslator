// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ResultTextEdit.h"

#include "QtCustom/SolMarkdownImporter.h"
#include "Types/SolGuard.h"
#include "Types/SolTypes.h"
#include "Utils/SolChrono.h"
#include "Utils/SolDocument.h"
#include "Utils/SolLog.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <quuid.h>


ResultTextEdit::ResultTextEdit(QWidget* parent) : MenuTextBrowser(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setReadOnly(true);

    QFont qfont = font();
    qfont.setPointSizeF(_fontSize);
    setFont(qfont);
    Qt::TextInteractionFlags interactionFlags = textInteractionFlags();
    interactionFlags.setFlag(Qt::TextSelectableByMouse);
    interactionFlags.setFlag(Qt::TextSelectableByKeyboard);
    interactionFlags.setFlag(Qt::LinksAccessibleByMouse);
    interactionFlags.setFlag(Qt::LinksAccessibleByKeyboard);
    setTextInteractionFlags(interactionFlags);
    ensureCursorVisible();
    setOpenExternalLinks(true);
    setOpenLinks(true);
}

ResultTextEdit::~ResultTextEdit()
{}

void ResultTextEdit::setFormattingText(const QString& inText, const TextStyle inTextStyle)
{
    switch (inTextStyle)
    {
    case TextStyle::None:
    case TextStyle::PlainText:
        setPlainText(inText);
        break;
    case TextStyle::Html:
        setHtml(inText);
        break;
    case TextStyle::MarkDown:
        setAdjustMarkdown(inText);
        break;
    case TextStyle::Size:
        break;
    default: ;
    }
}

void ResultTextEdit::setAdjustMarkdown(const QString& inMarkdownStr)
{
    // Change code blocks and links from markdown to html-style.
    // By converting to HTML, the code can be resized along with other text.
    // To prevent the '<>' inside the string from being recognized as tags, separate escape processing is done.

    QString md = inMarkdownStr;

    static const QRegularExpression codeBlockPattern(R"(```(.*?)```)", QRegularExpression::DotMatchesEverythingOption);
    static const QRegularExpression inlineCodePattern(R"(`(.*?)`)", QRegularExpression::DotMatchesEverythingOption);

    // ~=======================
    // Escape <>

    // Prevent <> escape in code area.
    static const QString blockPlaceMarker  = "__CODE_BLOCK_" + QUuid::createUuid().toString(QUuid::Id128) + "_%1__";
    static const QString inlinePlaceMarker = "__CODE_INLINE_" + QUuid::createUuid().toString(QUuid::Id128) + "_%1__";

    auto replaceCodeToMarker = [&md](const QRegularExpression& inRe, const QString& inKeyMarker)
    {
        QStringList resList;
        QString replaceStr;
        replaceStr.reserve(md.size());
        QRegularExpressionMatchIterator it = inRe.globalMatch(md);
        int lastPos = 0;
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();

            replaceStr += md.mid(lastPos, match.capturedStart() - lastPos);
            replaceStr += inKeyMarker.arg(resList.size());

            // 백틱 내부만 수집
            resList.append(match.captured(1));

            lastPos = match.capturedEnd();
        }
        replaceStr += md.mid(lastPos);
        md = std::move(replaceStr);
        return resList;
    };
    const QStringList codeBlockList = replaceCodeToMarker(codeBlockPattern, blockPlaceMarker);
    const QStringList inlineList    = replaceCodeToMarker(inlineCodePattern, inlinePlaceMarker);


    // Escape <> in outside of code. Except for <br/>.
    static const QRegularExpression unescapedLT(R"((?<!\\)<(?!br/>))");
    md.replace(unescapedLT, R"(\<)");
    static const QRegularExpression unescapedGT(R"((?<!\\|br/)>)");
    md.replace(unescapedGT, R"(\>)");


    // ~======================
    // Replace the code backticks with HTML-tags (for zoom) and restore the code.

    // Add monospace fonts
    const QStringList monoFontList = QStringList{"Cascadia Mono", "Consolas", "monospace"} + document()->defaultFont().families();
    QString codeFontFamilies = " font-family: ";
    for (const QString& font : monoFontList)
    {
        codeFontFamilies += "\'" + font + "\', ";
    }
    codeFontFamilies += ";";

    const QString codeStyle = getCodeBackgroundColorString() + codeFontFamilies ;

    static const QRegularExpression mdLinkPattern(R"(\[([^\]]+)\]\(([^)]+)\))");
    const QString codeLinkHtml = "<a href= \"\\2\"><code style= \"" + codeFontFamilies + " \" >\\1</code></a>";

    auto replaceMarkerToCode = [&codeLinkHtml, &md](const QStringList& inList, const QString& inCodeFormat, const QString& inPlaceMarker)
    {
        int lastIdx = 0;
        for (int idx = 0; idx < inList.size(); ++idx)
        {
            // Change the link to HTML-style.
            QString modifiedCode = inList[idx].toHtmlEscaped();
            modifiedCode.replace(mdLinkPattern, codeLinkHtml);
            modifiedCode = inCodeFormat.arg(modifiedCode);

            QString placeMarker = QString(inPlaceMarker).arg(idx);

            // searches only once from the previous point.
            const int pos = md.indexOf(placeMarker, lastIdx);
            if (pos != -1)
            {
                md.replace(pos, placeMarker.length(), modifiedCode);
                lastIdx = pos + modifiedCode.size();
            }
        }
    };

    const QString inlineCodeFormat = "<code style= \"" + codeStyle + "\">%1</code>";
    const QString blockCodeFormat  = "\n<pre style=\"" + codeStyle + "white-space: pre-wrap; \">\n%1\n</pre>";

    // Restore 'Marker To Code' in reverse order of 'Code To Marker'.
    replaceMarkerToCode(inlineList, inlineCodeFormat, inlinePlaceMarker);
    replaceMarkerToCode(codeBlockList, blockCodeFormat, blockPlaceMarker);

    if (md.startsWith("- <code") || md.startsWith("* <code") || md.startsWith("+ <code"))
    {
        md.push_front("<br/>\n\n");
    }

    SolMarkdownImporter mdImporter(document(), QTextDocument::MarkdownDialectGitHub);
    mdImporter.importMarkdown(md);
}

void ResultTextEdit::setCodeBackgroundColor(const QColor& inParam)
{
    _codeBackgroundColor = inParam;
    _codeBackgroundColorString
    = QString{"background-color: rgba(%1, %2, %3, %4);"}.arg(QString::number(_codeBackgroundColor.red())
                                                           , QString::number(_codeBackgroundColor.green())
                                                           , QString::number(_codeBackgroundColor.blue())
                                                           , QString::number(_codeBackgroundColor.alpha()));
}

QString ResultTextEdit::getCodeBackgroundColorString()
{
    if (_codeBackgroundColorString.isEmpty())
    {
        setCodeBackgroundColor(QColor(29, 29, 29, 255));
    }

    return _codeBackgroundColorString;
}
