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
#include "SubWidgets/SolWidgetFactory.h"

#include "Widgets/ui_TextEditTranslateWidget.h"


TextEditTranslateWidget::TextEditTranslateWidget(QWidget* parent)
    : ITranslateWidget(parent)
    , ui(new Ui::TextEditTranslateWidget)
{
    ui->setupUi(this);

    setLayout(ui->mainLayout);

    ui->TextEditLayout->setSpacing(8);

    ui->srcTextEdit->setAccessibleName(tr("번역 원문 입력 편집기"));
    ui->srcTextEdit->setTabChangesFocus(true);
    ui->srcTextEdit->setAcceptRichText(false);

    ui->trTextEdit->setAccessibleName(tr("번역 결과"));
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
    _srcLangSelector->setBubbleToolTip(tr("출발 언어"));
    connect(_srcLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onSourceLanguageChanged);

    ui->LangSelectLayout->insertWidget(0, _srcLangSelector, 1);


    // 도착언어 선택기
    _targetLangSelector = new LanguageSelector(this, ui->trTextEdit, ui->srcTextEdit, solConfig.getTextTargetLang());
    _targetLangSelector->setBubbleToolTip(tr("도착 언어"));
    connect(_targetLangSelector, &LanguageSelector::languageSelected, this, &TextEditTranslateWidget::onTargetLanguageChanged);

    ui->LangSelectLayout->insertWidget(2, _targetLangSelector, 1);


    // 언어 교환 버튼
    const bool bIsAuto = (solConfig.getTextSrcLang() == LangType::AUTO);
    ui->languageSwapButton->setEnabled(bIsAuto == false);
    ui->languageSwapButton->setFocusPolicy(Qt::TabFocus);
    ui->languageSwapButton->setAccessibleName(tr("언어 바꾸기"));
    ui->languageSwapButton->setAccessibleDescription(tr("출발 언어와 도착 언어를 서로 바꿉니다. 출발언어가 \'자동 감지\'라면 사용할 수 없습니다."));
    SolTooltipFilter::setBubbleToolTip(ui->languageSwapButton, tr("언어 바꾸기"));
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
        QPushButton* trRefresh = ui->trTextEdit->addBottomButton(QIcon(":/img/refresh_img")
                                                               , Qt::TabFocus
                                                               , tr("다시 번역")
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

QScrollBar* TextEditTranslateWidget::getVerticalScrollBar()
{
    return ui->trTextEdit->verticalScrollBar();
}

QScrollBar* TextEditTranslateWidget::getHorizontalScrollBar()
{
    return ui->trTextEdit->horizontalScrollBar();
}

QTextCursor TextEditTranslateWidget::getTextCursor()
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

void TextEditTranslateWidget::importExistingTranslation(const HistoryCacheData* inHistoryCache) const
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
    abortTrUnit();

    const QString orignText = ui->srcTextEdit->toPlainText();
    if (orignText.isEmpty())
    {
        ui->trTextEdit->setPlainText("");
        return;
    }
    ui->trTextEdit->setPlainText(tr("번역 중..."));

    solCore->translateManager()->translateText(TranslateRequestInfo{
        this
      , inIgnoreCache
      , solConfig.getCurrentEngineType()
      , orignText
      , TextStyle::PlainText
      , solConfig.getTextSrcLang()
      , solConfig.getTextTargetLang()
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

    if (solConfig.getTextSrcLang() != inlangType)
    {
        solConfig.setTextSrcLang(inlangType);
        _translationExecutionTimer->start();
    }
}

void TextEditTranslateWidget::onTargetLanguageChanged(const LangType inlangType) const
{
    if (solConfig.getTextTargetLang() != inlangType)
    {
        solConfig.setTextTargetLang(inlangType);
        _translationExecutionTimer->start();
    }
}

