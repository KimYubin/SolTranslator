// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TEXTEDITTRANSLATEWIDGET_H
#define TEXTEDITTRANSLATEWIDGET_H

#include "ITranslateWidget.h"

class HistoryCacheData;
class LanguageSelector;
class SolTranslatorCore;
enum class TextStyle;

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
    void applyTranslation() override;

    QScrollBar* getVerticalScrollBar() const override;
    QScrollBar* getHorizontalScrollBar() const override;
    QTextCursor getTextCursor() const override;
    void setTextCursor(const QTextCursor& cursor) override;

public:
    void focusTextOrigin();
    void importExistingTranslation(const HistoryCacheData* inHistoryCache);

private slots:
    void onExecuteTranslate(const bool inIgnoreCache = false);
    void onSourceLanguageChanged(const LangType inlangType);
    void onTargetLanguageChanged(const LangType inlangType);

private:
    Ui::TextEditTranslateWidget* ui;
    LanguageSelector* _srcLangSelector;
    LanguageSelector* _targetLangSelector;

    // 문자입력 후 번역요청까지 타이머
    QTimer* _translationExecutionTimer;
};


#endif //TEXTEDITTRANSLATEWIDGET_H
