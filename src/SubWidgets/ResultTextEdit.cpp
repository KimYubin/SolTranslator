// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "ResultTextEdit.h"

#include <QRegularExpression>
#include <QTextBlock>

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
    setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
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

    // 링크와 코드블록을 마크다운 스타일에서 html 스타일로 변경
    QString md = inMarkdownStr;

    // 이스케이프 되지 않은 <>가 태그로 인식되는 문제 해결
    static const QRegularExpression unescapedLT(R"((?<!\\)<)");
    md.replace(unescapedLT, R"(\<)");
    static const QRegularExpression unescapedGT(R"((?<!\\)>)");
    md.replace(unescapedGT, R"(\>)");

    QStringList monoFontList = doc->defaultFont().families();
    if (monoFontList.size() >= 2)
    {
        monoFontList.swapItemsAt(0, 1);
    }

    QString codeFontFamilies = " font-family: ";
    for (QString& font : monoFontList)
    {
        codeFontFamilies += "\'" + font + "\', ";
    }
    codeFontFamilies += ";";

    static const QRegularExpression mdLinkPattern(R"(\[([^\]]+)\]\(([^)]+)\))");

    // 문단 코드
    static const QRegularExpression codeQuotingPattern("```(.*?)```", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = codeQuotingPattern.globalMatch(md);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();

        QString original  = match.captured(0); // 백틱 포함 전체 패턴 일치
        QString codeBlock = match.captured(1); // 백틱 내부만

        // 코드 내부에 링크가 있다면, html 스타일 링크로 변경
        QString modified = codeBlock.toHtmlEscaped();
        modified.replace(mdLinkPattern, "<a href=\"\\2\"><code style= \"" + codeFontFamilies + " \"" " >\\1</code></a>");

        // 원래 코드 블록 전체를 수정된 내용으로 대체
        // 백틱을 html 스타일 코드 인용으로 변경
        md.replace(original
                 , "\n<pre style=\"white-space: pre-wrap; background-color:" + codeBgColorStr + "; " + codeFontFamilies + " \">\n" + modified +
                   "</pre>");
    }

    // 단어 코드 스니펫
    static const QRegularExpression codePattern("`(.*?)`", QRegularExpression::DotMatchesEverythingOption);
    it = codePattern.globalMatch(inMarkdownStr);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();

        QString original  = match.captured(0); // 백틱 포함 전체 패턴 일치
        QString codeBlock = match.captured(1); // 백틱 내부만

        // 코드 내부에 링크가 있다면, html 스타일 링크로 변경
        QString modified = codeBlock.toHtmlEscaped();
        modified.replace(mdLinkPattern, "<a href=\"\\2\"><code style= \"" + codeFontFamilies + " \"" " >\\1</code></a>");

        // 원래 코드 블록 전체를 수정된 내용으로 대체
        // 백틱을 html 스타일 코드 인용으로 변경
        md.replace(original, "<code style= \"" + codeFontFamilies + "background-color:" + codeBgColorStr + "; \">" + modified + "</code>");
    }

    doc->setMarkdown(md);
}
