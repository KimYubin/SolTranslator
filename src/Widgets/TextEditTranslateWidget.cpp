//
// Created by YubinKim on 25/03/19 수.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TextEditTranslateWidget.h" resolved

#include "TextEditTranslateWidget.h"

#include <QLineEdit>
#include <QPointer>
#include <QScrollBar>
#include <QTimer>

#include <qsortfilterproxymodel.h>
#include <qstringlistmodel.h>

#include "FinTranslatorCore.h"
#include "FinTypes.h"
#include "Managers/ConfigManager.h"
#include "Managers/TranslateManager.h"

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
        Qt::LinksAccessibleByKeyboard |
        Qt::TextEditable
    );

    // 출발언어 선택기
    _sourceLang = new LanguageSelector(this, ui->srcTextEdit, finConfig.getTextSrcLang());
    connect(_sourceLang, &LanguageSelector::languageSelected, this, [this](const LangType inlangType)
    {
        const bool bIsAuto = (inlangType == LangType::AUTO);
        ui->languageSwapButton->setEnabled(bIsAuto == false);

        finConfig.setTextSrcLang(inlangType);
    });

    ui->LangSelectLayout->insertWidget(0, _sourceLang, 1);


    // 도착언어 선택기
    _targetLang = new LanguageSelector(this, ui->trTextEdit, finConfig.getTextTargetLang());
    connect(_targetLang, &LanguageSelector::languageSelected, &finConfig, &ConfigManager::setTextTargetLang);

    ui->LangSelectLayout->insertWidget(2, _targetLang, 1);


    // 언어 교환 버튼
    const bool bIsAuto = (finConfig.getTextSrcLang() == LangType::AUTO);
    ui->languageSwapButton->setEnabled(bIsAuto == false);
    connect(ui->languageSwapButton, &QPushButton::clicked, this, [this]()
    {
        const LangType srcLangType    = finConfig.getTextSrcLang();
        const LangType targetLangType = finConfig.getTextTargetLang();
        if (srcLangType == LangType::AUTO)
        {
            qDebug()<<"swap button is clicked, when source Language Type is AUTO.";
            return;
        }

        _sourceLang->onSelectedLanguage(targetLangType);
        _targetLang->onSelectedLanguage(srcLangType);
    });

    // 번역 실행 타이머
    _translationExecutionTimer = new QTimer(this);
    _translationExecutionTimer->setInterval(500);
    _translationExecutionTimer->setSingleShot(true);
    connect(_translationExecutionTimer, &QTimer::timeout, this, &TextEditTranslateWidget::onTranslateClicked);
    connect(ui->srcTextEdit, &QPlainTextEdit::textChanged, this, [this]()
    {
        _translationExecutionTimer->start();
    });

    
    // todo: 검색 기능 추가 예정.
    // ui->OrignLangSelectCombo->setEditable(true);
    //
    // QStringList items       = {"Apple", "Banana", "Cherry", "Date"};
    // QStringListModel* model = new QStringListModel(items, this);
    //
    // QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    // proxyModel->setSourceModel(model);
    // proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    //
    // ui->OrignLangSelectCombo->setModel(proxyModel);
    //
    // connect(ui->OrignLangSelectCombo->lineEdit(), &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);
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

void TextEditTranslateWidget::onTranslateClicked()
{
    const QString orignText = ui->srcTextEdit->toPlainText();

    finCore->getTranslateManager()->translateText(TranslateRequestInfo{
        orignText
      , TextStyle::PlainText
      , finConfig.getTextSrcLang()
      , finConfig.getTextTargetLang()
      , ui->trTextEdit
      , [this](const QString& inStr) { completeTransText(inStr, TextStyle::PlainText); }
      , ui->trTextEdit
      , [this](const QString& inStr) { streamTransText(inStr, TextStyle::PlainText); }
    });
}

