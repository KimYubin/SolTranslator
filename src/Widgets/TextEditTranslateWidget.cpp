// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "TextEditTranslateWidget.h"

#include <QClipboard>
#include <QLineEdit>
#include <QMenu>
#include <QScrollBar>
#include <QTimer>

#include "HistoryCacheData.h"
#include "SolLog.h"
#include "SolTranslatorCore.h"
#include "SolMainWidget.h"
#include "SolTypes.h"
#include "SolUtilibrary.h"

#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

#include "SubWidgets/SolToast.h"
#include "SubWidgets/SolToolTip.h"
#include "SubWidgets/LanguageSelector.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolWidgetFactory.h"

#include "Utils/Tr.h"

#include "Widgets/ui_TextEditTranslateWidget.h"

using Sol::i18n;

TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    ui->TextEditLayout->setSpacing(8);

    ui->srcTextEdit->setAccessibleName(i18n(Tr::Source_Text_Editor));
    ui->srcTextEdit->setTabChangesFocus(true);
    ui->srcTextEdit->setAcceptRichText(false);

    ui->trTextEdit->setAccessibleName(i18n(Tr::Translation_Result));
    ui->trTextEdit->setTabChangesFocus(true);
    ui->trTextEdit->setReadOnly(true);
    ui->trTextEdit->setMouseTracking(false);
    ui->trTextEdit->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard //|
        // Qt::LinksAccessibleByMouse |
        // Qt::LinksAccessibleByKeyboard
    );


    // 출발언어 선택기
    _srcLangSelector = new LanguageSelector(this, ui->srcTextEdit, ui->srcTextEdit, solConfig.getTextSrcLang());
    _srcLangSelector->setBubbleToolTip(i18n(Tr::Source_Language));
    connect(_srcLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onSourceLanguageChanged);

    ui->LangSelectLayout->insertWidget(0, _srcLangSelector, 1);


    // 도착언어 선택기
    _targetLangSelector = new LanguageSelector(this, ui->trTextEdit, ui->srcTextEdit, solConfig.getTextTargetLang());
    _targetLangSelector->setBubbleToolTip(i18n(Tr::Target_Language));
    connect(_targetLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onTargetLanguageChanged);

    ui->LangSelectLayout->insertWidget(2, _targetLangSelector, 1);


    // 언어 교환 버튼
    const bool isAuto = (solConfig.getTextSrcLang() == LangType::AUTO);
    ui->languageSwapButton->setEnabled(isAuto == false);
    ui->languageSwapButton->setFocusPolicy(Qt::TabFocus);
    ui->languageSwapButton->setAccessibleName(i18n(Tr::Swap_Language));
    ui->languageSwapButton->setAccessibleDescription(i18n(Tr::Swap_Language_Desc));
    SolTooltipFilter::setBubbleToolTip(ui->languageSwapButton, i18n(Tr::Swap_Language));
    connect(ui->languageSwapButton, &QPushButton::clicked, this, [this]()
    {
        const LangType srcLangType    = solConfig.getTextSrcLang();
        const LangType targetLangType = solConfig.getTextTargetLang();
        if (srcLangType == LangType::AUTO)
        {
            solDebug << "swap button is clicked, when source Language Type is AUTO.";
            return;
        }

        ui->srcTextEdit->setPlainText(ui->trTextEdit->toPlainText());
        ui->trTextEdit->setPlainText("");

        _srcLangSelector->onSelectedLanguage(targetLangType);
        _targetLangSelector->onSelectedLanguage(srcLangType);
    });

    {
        // 전체 복사 버튼
        QPushButton* trCopy = SolWidgetFactory::createCopyButton(this, [this]() { return getTranslatedText(); });
        ui->trTextEdit->addBottomWidget(trCopy, 0, Qt::AlignLeft);
    }
    {
        // 다시 번역 버튼
        SolButton* trRefresh = ui->trTextEdit->addBottomButton(QIcon(":/img/refresh_img")
                                                             , Qt::TabFocus
                                                             , i18n(Tr::Re_Translate)
                                                             , QKeySequence()
                                                             , 0
                                                             , Qt::AlignRight);

        connect(trRefresh, &QPushButton::clicked, this, [this]()
        {
            QTimer::singleShot(500, this, [this]()
            {
                onExecuteTranslate(true);
            });
        });
    }


    // 번역 실행 타이머
    _translationExecutionTimer = new QTimer(this);
    _translationExecutionTimer->setInterval(500);
    _translationExecutionTimer->setSingleShot(true);
    connect(_translationExecutionTimer, &QTimer::timeout, this, [this]() { onExecuteTranslate(false); });
    connect(ui->srcTextEdit, &QTextEdit::textChanged, this, [this]()
    {
        _translationExecutionTimer->start();
    });

    setTabOrder({_srcLangSelector, ui->srcTextEdit, ui->languageSwapButton, _targetLangSelector, ui->trTextEdit});
    setFocusProxy(_srcLangSelector);
}

TextEditTranslateWidget::~TextEditTranslateWidget()
{
    delete ui;
}

void TextEditTranslateWidget::applyTranslation()
{
    ui->trTextEdit->setPlainText(getTranslatedText());
}

QScrollBar* TextEditTranslateWidget::getVerticalScrollBar() const
{
    return ui->trTextEdit->verticalScrollBar();
}

QScrollBar* TextEditTranslateWidget::getHorizontalScrollBar() const
{
    return ui->trTextEdit->horizontalScrollBar();
}

QTextCursor TextEditTranslateWidget::getTextCursor() const
{
    return ui->trTextEdit->textCursor();
}

void TextEditTranslateWidget::setTextCursor(const QTextCursor& cursor)
{
    ui->trTextEdit->setTextCursor(cursor);
}

void TextEditTranslateWidget::focusTextOrigin()
{
    ui->srcTextEdit->setFocus();
}

void TextEditTranslateWidget::importExistingTranslation(const HistoryCacheData* inHistoryCache)
{
    if (inHistoryCache == nullptr)
    {
        solDebug << "inHistoryCache == nullptr";
        return;
    }
    ui->srcTextEdit->setText(inHistoryCache->getSourceText());
    ui->trTextEdit->setFormattingText(inHistoryCache->getTargetText(), inHistoryCache->getTextStyle());
}

void TextEditTranslateWidget::onExecuteTranslate(const bool inIgnoreCache)
{
    detachTrUnit();

    const QString orignText = ui->srcTextEdit->toPlainText();
    if (orignText.isEmpty())
    {
        ui->trTextEdit->setPlainText("");
        return;
    }
    ui->trTextEdit->setPlainText(i18n(Tr::Translating));

    solCore->translateManager()->translateText(TranslateRequestInfo{
        this
      , inIgnoreCache
      , solConfig.getCurrentEngineType()
      , orignText
      , TextStyle::PlainText
      , solConfig.getTextSrcLang()
      , solConfig.getTextTargetLang()
      , this
      , [this](const QString& inStr) { completeTransText(inStr, TextStyle::PlainText); }
      , this
      , [this](const QString& inStr) { streamTransText(inStr, TextStyle::PlainText); }
    });
}

void TextEditTranslateWidget::onSourceLanguageChanged(const LangType inlangType)
{
    const bool isAuto = (inlangType == LangType::AUTO);
    ui->languageSwapButton->setEnabled(isAuto == false);

    if (solConfig.getTextSrcLang() != inlangType)
    {
        solConfig.setTextSrcLang(inlangType);
        _translationExecutionTimer->start();
    }
}

void TextEditTranslateWidget::onTargetLanguageChanged(const LangType inlangType)
{
    if (solConfig.getTextTargetLang() != inlangType)
    {
        solConfig.setTextTargetLang(inlangType);
        _translationExecutionTimer->start();
    }
}

