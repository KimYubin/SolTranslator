// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TextEditTranslateWidget.h"

#include "HistoryCacheData.h"
#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"
#include "SubWidgets/LanguageSelector.h"
#include "SubWidgets/SolButton.h"
#include "SubWidgets/SolToolTip.h"
#include "SubWidgets/SolWidgetFactory.h"
#include "Types/EngineId.h"
#include "Types/SolTypes.h"
#include "Utils/SolI18n.h"
#include "Utils/SolLog.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/ui_TextEditTranslateWidget.h"

#include <QLineEdit>
#include <QMenu>
#include <QScrollBar>
#include <QTimer>

using Sol::i18n;

TextEditTranslateWidget::TextEditTranslateWidget(QWidget* inParent)
    : ITranslateWidget(inParent)
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
    _srcLangSelector = new LanguageSelector(this, ui->srcTextEdit, ui->srcTextEdit, solConfig.textSrcLang());
    _srcLangSelector->setButtonToolTip(i18n(Tr::Source_Language));
    connect(_srcLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onSourceLanguageChanged);

    ui->LangSelectLayout->insertWidget(0, _srcLangSelector, 1);


    // 도착언어 선택기
    _targetLangSelector = new LanguageSelector(this, ui->trTextEdit, ui->srcTextEdit, solConfig.textTargetLang());
    _targetLangSelector->setButtonToolTip(i18n(Tr::Target_Language));
    connect(_targetLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onTargetLanguageChanged);

    ui->LangSelectLayout->insertWidget(2, _targetLangSelector, 1);


    // 언어 교환 버튼
    const bool isAuto = (solConfig.textSrcLang() == LangType::AUTO);
    ui->languageSwapButton->setEnabled(isAuto == false);
    ui->languageSwapButton->setFocusPolicy(Qt::TabFocus);
    ui->languageSwapButton->setAccessibleName(i18n(Tr::Swap_Language));
    ui->languageSwapButton->setAccessibleDescription(i18n(Tr::Swap_Language_Desc));
    SolToolTip::setToolTip(ui->languageSwapButton, i18n(Tr::Swap_Language));
    connect(ui->languageSwapButton, &QPushButton::clicked, this, [this]()
    {
        const LangType srcLangType    = solConfig.textSrcLang();
        const LangType targetLangType = solConfig.textTargetLang();
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
        SolButton* trCopy = SolWidgetFactory::createCopyButton(this, [this]() { return getTargetText(); });
        ui->trTextEdit->addBottomWidget(trCopy, 0, Qt::AlignLeft);
    }
    {
        // 다시 번역 버튼
        SolButton* reTr = SolWidgetFactory::createReTranslateButton(this, [this]()
        {
            QTimer::singleShot(500, this, [this]() { onExecuteTranslate(true); });
        });
        ui->trTextEdit->addBottomWidget(reTr, 0, Qt::AlignRight);
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
    ui->trTextEdit->setPlainText(getTargetText());
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

void TextEditTranslateWidget::setTextCursor(const QTextCursor& inCursor)
{
    ui->trTextEdit->setTextCursor(inCursor);
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

    const QString sourceText = ui->srcTextEdit->toPlainText();
    if (sourceText.isEmpty())
    {
        ui->trTextEdit->setPlainText("");
        return;
    }
    ui->trTextEdit->setPlainText(i18n(Tr::Translating));

    executeTranslateImpl(solConfig.currentEngineId()
                       , sourceText
                       , TextStyle::PlainText
                       , solConfig.textSrcLang()
                       , solConfig.textTargetLang()
                       , inIgnoreCache);
}

void TextEditTranslateWidget::onSourceLanguageChanged(const LangType inLangType)
{
    const bool isAuto = (inLangType == LangType::AUTO);
    ui->languageSwapButton->setEnabled(isAuto == false);

    if (solConfig.textSrcLang() != inLangType)
    {
        solConfig.setTextSrcLang(inLangType);
        _translationExecutionTimer->start();
    }
}

void TextEditTranslateWidget::onTargetLanguageChanged(const LangType inLangType)
{
    if (solConfig.textTargetLang() != inLangType)
    {
        solConfig.setTextTargetLang(inLangType);
        _translationExecutionTimer->start();
    }
}

