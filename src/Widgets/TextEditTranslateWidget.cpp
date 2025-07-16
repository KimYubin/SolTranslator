// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TextEditTranslateWidget.h"

#include <QLineEdit>
#include <QScrollBar>
#include <QTimer>

#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/FinToolTip.h"
#include "SubWidgets/LanguageSelector.h"

#include "Widgets/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    ui->TextEditLayout->setSpacing(8);

    ui->srcTextEdit->setTabChangesFocus(true);
    ui->trTextEdit->setTabChangesFocus(true);
    ui->trTextEdit->setReadOnly(true);
    ui->trTextEdit->setMouseTracking(false);
    ui->trTextEdit->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard
    );

    // 출발언어 선택기
    _srcLangSelector = new LanguageSelector(this, ui->srcTextEdit, finConfig.getTextSrcLang());
    _srcLangSelector->setBubbleToolTip(tr("출발 언어"));
    connect(_srcLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onSourceLanguageChanged);

    ui->LangSelectLayout->insertWidget(0, _srcLangSelector, 1);


    // 도착언어 선택기
    _targetLangSelector = new LanguageSelector(this, ui->trTextEdit, finConfig.getTextTargetLang());
    _targetLangSelector->setBubbleToolTip(tr("도착 언어"));
    connect(_targetLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onTargetLanguageChanged);

    ui->LangSelectLayout->insertWidget(2, _targetLangSelector, 1);


    // 언어 교환 버튼
    const bool bIsAuto = (finConfig.getTextSrcLang() == LangType::AUTO);
    ui->languageSwapButton->setEnabled(bIsAuto == false);
    FinTooltipFilter::setBubbleToolTip(ui->languageSwapButton, tr("언어 바꾸기"));
    connect(ui->languageSwapButton, &QPushButton::clicked, this, [this]()
    {
        const LangType srcLangType    = finConfig.getTextSrcLang();
        const LangType targetLangType = finConfig.getTextTargetLang();
        if (srcLangType == LangType::AUTO)
        {
            qDebug() << "swap button is clicked, when source Language Type is AUTO.";
            return;
        }

        ui->srcTextEdit->setPlainText(ui->trTextEdit->toPlainText());
        ui->trTextEdit->setPlainText("");

        _srcLangSelector->onSelectedLanguage(targetLangType);
        _targetLangSelector->onSelectedLanguage(srcLangType);
    });


    // 번역 실행 타이머
    _translationExecutionTimer = new QTimer(this);
    _translationExecutionTimer->setInterval(500);
    _translationExecutionTimer->setSingleShot(true);
    connect(_translationExecutionTimer, &QTimer::timeout, this, &TextEditTranslateWidget::onExecuteTranslate);
    connect(ui->srcTextEdit, &QPlainTextEdit::textChanged, this, [this]()
    {
        _translationExecutionTimer->start();
    });

    setTabOrder({_srcLangSelector, ui->srcTextEdit, ui->languageSwapButton, _targetLangSelector, ui->trTextEdit});
}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    ui->trTextEdit->setPlainText(inTranslatedText);
}

QScrollBar* TextEditTranslateWidget::getVerticalScrollBar()
{
    return ui->trTextEdit->verticalScrollBar();
}

QScrollBar* TextEditTranslateWidget::getHorizontalScrollBar()
{
    return ui->trTextEdit->horizontalScrollBar();
}

void TextEditTranslateWidget::focusTextOrigin()
{
    ui->srcTextEdit->setFocus();
}

void TextEditTranslateWidget::onExecuteTranslate()
{
    const QString orignText = ui->srcTextEdit->toPlainText();
    if (orignText.isEmpty())
    {
        ui->trTextEdit->setPlainText("");
        return;
    }
    ui->trTextEdit->setPlainText(tr("번역 중..."));

    finCore->getTranslateManager()->translateText(TranslateRequestInfo{
        finConfig.getCurrentEngineType()
      , orignText
      , TextStyle::PlainText
      , finConfig.getTextSrcLang()
      , finConfig.getTextTargetLang()
      , ui->trTextEdit
      , [this](const QString& inStr) { completeTransText(inStr, TextStyle::PlainText); }
      , ui->trTextEdit
      , [this](const QString& inStr) { streamTransText(inStr, TextStyle::PlainText); }
    });
}

void TextEditTranslateWidget::onSourceLanguageChanged(const LangType inlangType) const
{
    const bool bIsAuto = (inlangType == LangType::AUTO);
    ui->languageSwapButton->setEnabled(bIsAuto == false);

    if (finConfig.getTextSrcLang() != inlangType)
    {
        finConfig.setTextSrcLang(inlangType);
        _translationExecutionTimer->start();
    }
}

void TextEditTranslateWidget::onTargetLanguageChanged(const LangType inlangType) const
{
    if (finConfig.getTextTargetLang() != inlangType)
    {
        finConfig.setTextTargetLang(inlangType);
        _translationExecutionTimer->start();
    }
}

