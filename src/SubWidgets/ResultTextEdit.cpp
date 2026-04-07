// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ResultTextEdit.h"

#include "SolTypes.h"
#include "Utils/SolLog.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <quuid.h>

namespace
{
constexpr QColor codeBgColor(29, 29, 29, 255);

const QString codeBgColorStr = QString::fromLatin1("rgba(%1,%2,%3,%4)")
                               .arg(codeBgColor.red())
                               .arg(codeBgColor.green())
                               .arg(codeBgColor.blue())
                               .arg(codeBgColor.alpha());
} // anonymous namespace

ResultTextEdit::ResultTextEdit(QWidget* parent) : MenuTextBrowser(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setReadOnly(true);

    QFont qfont = font();
    qfont.setPointSizeF(_fontSize);
    setFont(qfont);
    Qt::TextInteractionFlags interactionFlags = textInteractionFlags();
    interactionFlags.setFlag(Qt::TextInteractionFlag::TextSelectableByMouse);
    interactionFlags.setFlag(Qt::TextInteractionFlag::TextSelectableByKeyboard);
    interactionFlags.setFlag(Qt::TextInteractionFlag::LinksAccessibleByMouse);
    interactionFlags.setFlag(Qt::TextInteractionFlag::LinksAccessibleByKeyboard);
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
        setText(inText);
        break;
    case TextStyle::Html:
        setHtml(inText);
        break;
    case TextStyle::MarkDown:
    {
        setAdjustMarkdown(inText);
        break;
    }
    case TextStyle::Size:
        break;
    default: ;
    }

    // 문단간 간격 조정.
    const QFontMetricsF fntMetricsF(font());
    const qreal lineHeight = fntMetricsF.lineSpacing();
    const qreal parSpacing = lineHeight * 0.6; // 줄간격의 1.6배

    QTextBlock block = document()->firstBlock();

    while (block.isValid() && block.next().isValid())
    {
        QTextCursor blockCursor(block);
        QTextBlockFormat blockFormat = blockCursor.blockFormat();

        // 코드 블록은 간격 조정 안함.
        if (blockFormat.background().color() == codeBgColor
            && block.next().blockFormat().background().color() == codeBgColor)
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
    QTextDocument* doc = document();

    // Change code blocks and links from markdown to html-style.
    QString md = inMarkdownStr;

    static const QRegularExpression codeQuotingPattern(R"(```(.*?)```)", QRegularExpression::DotMatchesEverythingOption);
    static const QRegularExpression inlineCodePattern(R"(`(.*?)`)", QRegularExpression::DotMatchesEverythingOption);

    // ~=======================
    // Escape <>

    // Prevent <> escape in code area.
    static const QString codePlaceMarker = "__CODE_" + QUuid::createUuid().toString(QUuid::Id128) + "_%1__";

    QStringList codeBlocks;
    auto rePlaceCode = [&md, &codeBlocks](const QRegularExpression& re)
    {
        QString replaceStr;
        replaceStr.reserve(md.size());
        QRegularExpressionMatchIterator it = re.globalMatch(md);
        int lastPos = 0;
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();

            replaceStr += md.mid(lastPos, match.capturedStart() - lastPos);
            replaceStr += QString(codePlaceMarker).arg(codeBlocks.size());

            // 백틱 내부만 수집
            codeBlocks.append(match.captured(1));

            lastPos = match.capturedEnd();
        }
        replaceStr += md.mid(lastPos);
        md = std::move(replaceStr);
    };
    rePlaceCode(codeQuotingPattern);
    const int inlineStartIdx = codeBlocks.size();
    rePlaceCode(inlineCodePattern);

    // Escape <> in outside of code.
    static const QRegularExpression unescapedLT(R"((?<!\\)<)");
    md.replace(unescapedLT, R"(\<)");
    static const QRegularExpression unescapedGT(R"((?<!\\)>)");
    md.replace(unescapedGT, R"(\>)");


    // ~======================
    // Replace the code backticks with HTML-tags (for zoom) and restore the code.

    // monospace font families
    QStringList monoFontList = {"Cascadia Mono", "Consolas", "monospace"}; 
    monoFontList += doc->defaultFont().families();
    QString codeFontFamilies = " font-family: ";
    for (QString& font : monoFontList)
    {
        codeFontFamilies += "\'" + font + "\', ";
    }
    codeFontFamilies += ";";

    static const QRegularExpression mdLinkPattern(R"(\[([^\]]+)\]\(([^)]+)\))");

    // Change to HTML-style and restore the codes.
    for (int idx = 0; idx < codeBlocks.size(); ++idx)
    {
        QString placeMarker = QString(codePlaceMarker).arg(idx);

        // Change the link in backticks to HTML-style.
        QString modifiedCode = codeBlocks[idx].toHtmlEscaped();
        modifiedCode.replace(mdLinkPattern, "<a href=\"\\2\"><code style= \"" + codeFontFamilies + " \"" " >\\1</code></a>");

        // Change backticks to HTML-style code quotes.
        if (idx < inlineStartIdx)
        {
            // 문단 코드
            modifiedCode =
                    "\n<pre style=\"white-space: pre-wrap; background-color:" + codeBgColorStr + "; " + codeFontFamilies + " \">\n"
                    + modifiedCode
                    + "</pre>";
        }
        else
        {
            // 단어 코드 스니펫
            modifiedCode =
                    "<code style= \"" + codeFontFamilies + "background-color:" + codeBgColorStr + "; \">"
                    + modifiedCode
                    + "</code>";
        }

        const int pos = md.indexOf(placeMarker);
        if (pos != -1) 
        {
            md.replace(pos, placeMarker.length(), modifiedCode);
        }

        // md.replace(placeMarker, modifiedCode);
    }

    doc->setMarkdown(md);
}
