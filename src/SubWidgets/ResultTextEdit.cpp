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
        break;
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

    // 문단간 간격 조정.
    const QFontMetricsF fntMetricsF(font());
    const qreal lineHeight = fntMetricsF.lineSpacing();
    const qreal parSpacing = lineHeight * 0.6; // 줄간격의 1.6배

    QTextCursor blockCursor(document());
    TextCursorEditBlockGuard cursorEditBlockGuard{blockCursor};

    QTextBlock block = document()->firstBlock();
    while (block.isValid() && block.next().isValid())
    {
        blockCursor.setPosition(block.position());
        QTextBlockFormat blockFormat = blockCursor.blockFormat();

        // 코드 블록은 간격 조정 안함.
        if (blockFormat.background().color() == getCodeBackgroundColor()
            && block.next().blockFormat().background().color() == getCodeBackgroundColor())
        {
            break;
        }

        blockFormat.setBottomMargin(parSpacing);
        blockCursor.setBlockFormat(blockFormat);

        block = block.next();
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
            replaceStr += QString(inKeyMarker).arg(resList.size());

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
    document()->setProperty("monoFontList", monoFontList);
    QString codeFontFamilies = " font-family: ";
    for (const QString& font : monoFontList)
    {
        codeFontFamilies += "\'" + font + "\', ";
    }
    codeFontFamilies += ";";

    const QString codeStyle = getCodeBackgroundColorString() + codeFontFamilies ;

    static const QRegularExpression mdLinkPattern(R"(\[([^\]]+)\]\(([^)]+)\))");
    const QString codeLinkHtml     = "<a href= \""   "\\2"   "\"><code style= \"" + codeFontFamilies + " \" >"   "\\1"   "</code></a>";
    const QString blockCodeFormat  = "\n<pre style=\" " + codeStyle + " white-space: pre-wrap; \">\n"  "%1"  "</pre>";
    const QString inlineCodeFormat = "<code style= \" " + codeStyle + " \">"  "%1"  "</code>";

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

    // Restore 'Marker To Code' in reverse order of 'Code To Marker'.
    replaceMarkerToCode(inlineList, inlineCodeFormat, inlinePlaceMarker);
    replaceMarkerToCode(codeBlockList, blockCodeFormat, blockPlaceMarker);

    if (md.startsWith("- <code") || md.startsWith("* <code") || md.startsWith("+ <code"))
    {
        md.push_front("<br/>\n\n");
    }

    // document()->setMarkdown(md);
    SolMarkdownImporter(document(), QTextDocument::MarkdownDialectGitHub).import(md);

    QFont monoFont;//(monoFontList);
    QTextCursor cursor(document());
    for (QTextBlock textBlock = document()->begin(); textBlock.isValid(); textBlock = textBlock.next())
    {
        QTextBlockFormat blockFmt = textBlock.blockFormat();
        if (blockFmt.hasProperty(QTextFormat::BlockCodeLanguage))
        {
            QTextCharFormat charFormat;// = textBlock.charFormat();
            charFormat.setFont(monoFont);
            blockFmt.setBackground(getCodeBackgroundColor());
            if (blockFmt.nonBreakableLines())
            {
                blockFmt.setNonBreakableLines(false);
            }

            cursor.setPosition(textBlock.position());
            cursor.setPosition(textBlock.position() + textBlock.length(), QTextCursor::KeepAnchor);
            cursor.setBlockFormat(blockFmt);
            cursor.setBlockCharFormat(charFormat);
            continue;
        }

        for (QTextBlock::iterator blockIt = textBlock.begin(); !blockIt.atEnd(); ++blockIt)
        {
            QTextFragment frag = blockIt.fragment();
            if (frag.charFormat().hasProperty(QTextFormat::UserProperty + 1))
            {
                QTextCharFormat charFormat;// = frag.charFormat();
                charFormat.setBackground(getCodeBackgroundColor());
                charFormat.setFont(monoFont);

                cursor.setPosition(frag.position());
                cursor.setPosition(frag.position()+ frag.length(), QTextCursor::KeepAnchor);
                cursor.setCharFormat(charFormat);
            }
        }
    }
}

void ResultTextEdit::setCodeBackgroundColor(const QColor& inParam)
{
    _codeBackgroundColor = inParam;

    _codeBackgroundColorString
            = "background-color: "
            + QString{"rgba(%1, %2, %3, %4)"}
              .arg(_codeBackgroundColor.red())
              .arg(_codeBackgroundColor.green())
              .arg(_codeBackgroundColor.blue())
              .arg(_codeBackgroundColor.alpha())
            + "; ";
}

QString ResultTextEdit::getCodeBackgroundColorString()
{
    if (_codeBackgroundColorString.isEmpty())
    {
        setCodeBackgroundColor(QColor(29, 29, 29, 255));
    }

    return _codeBackgroundColorString;
}
