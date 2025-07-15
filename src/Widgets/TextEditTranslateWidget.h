// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TEXTEDITTRANSLATEWIDGET_H
#define TEXTEDITTRANSLATEWIDGET_H

#include <QWidget>

#include "ITranslateWidget.h"


class LanguageSelector;
enum class TextStyle;
class FinTranslatorCore;
QT_BEGIN_NAMESPACE

namespace Ui
{
class TextEditTranslateWidget;
}

QT_END_NAMESPACE

class TextEditTranslateWidget : public ITranslateWidget
{
    Q_OBJECT

public:
    explicit TextEditTranslateWidget(QWidget* parent = nullptr);
    ~TextEditTranslateWidget() override;

protected:
    /** 입력된 문자열로 교체하고, 적정 사이즈로 팝업을 엽니다. */
    virtual void applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle) override;

    virtual QScrollBar* getVerticalScrollBar() override;
    virtual QScrollBar* getHorizontalScrollBar() override;

public:
    void focusTextOrigin();

private slots:
    void onExecuteTranslate();
    void onSourceLanguageChanged(const LangType inlangType) const;
    void onTargetLanguageChanged(const LangType inlangType) const;

private:
    Ui::TextEditTranslateWidget* ui;
    LanguageSelector* _srcLangSelector;
    LanguageSelector* _targetLangSelector;

    // 문자입력 후 번역요청까지 타이머
    QTimer* _translationExecutionTimer;
};


#endif //TEXTEDITTRANSLATEWIDGET_H
