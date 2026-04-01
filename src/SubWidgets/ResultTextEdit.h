// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_RESULTTEXTEDIT_H
#define SOLTRANSLATOR_RESULTTEXTEDIT_H

#include "CustomMenuTextEdit.h"

enum class TextStyle;

class ResultTextEdit : public MenuTextBrowser
{
    Q_OBJECT
public:
    explicit ResultTextEdit(QWidget* parent = nullptr);

    ~ResultTextEdit() override;

    /**
     * 텍스트 스타일에 맞춰 서식이 지정된 텍스트를 적용합니다.
     */
    void setFormattingText(const QString& inText, const TextStyle inTextStyle);

protected:
    /**
     * 마크다운 string의 스타일 일부를 HTML로 재조정한 후 적용합니다.
     * setMarkdown을 대체합니다.
     */
    void setAdjustMarkdown(const QString& inMarkdownStr);

private:
    qreal _fontSize = 14.0;
};



#endif //SOLTRANSLATOR_RESULTTEXTEDIT_H
