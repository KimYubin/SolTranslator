// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef TEXTEDITTRANSLATEWIDGET_H
#define TEXTEDITTRANSLATEWIDGET_H

#include "ITranslateWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class TextEditTranslateWidget;
}
QT_END_NAMESPACE

namespace Sol
{
class HistoryCacheData;
class LanguageSelector;
class SolTranslatorCore;
enum class TextStyle;

class TextEditTranslateWidget : public ITranslateWidget
{
    Q_OBJECT

public:
    explicit TextEditTranslateWidget(QWidget* inParent = nullptr);
    ~TextEditTranslateWidget() override;

protected:
    void applyTranslation() override;

    QScrollBar* getVerticalScrollBar() const override;
    QScrollBar* getHorizontalScrollBar() const override;
    QTextCursor getTextCursor() const override;
    void setTextCursor(const QTextCursor& inCursor) override;

public:
    void focusTextOrigin();
    void importExistingTranslation(const HistoryCacheData* inHistoryCache);

    void onExecuteTranslate(const bool inIgnoreCache = false);
    void onSourceLanguageChanged(const LangType inLangType);
    void onTargetLanguageChanged(const LangType inLangType);

private:
    Ui::TextEditTranslateWidget* ui;
    LanguageSelector* _srcLangSelector;
    LanguageSelector* _targetLangSelector;

    // 문자입력 후 번역요청까지 타이머
    QTimer* _translationExecutionTimer;
};


} // namespace Sol

#endif //TEXTEDITTRANSLATEWIDGET_H
