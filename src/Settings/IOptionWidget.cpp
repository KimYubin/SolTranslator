// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "IOptionWidget.h"

#include <QGroupBox>
#include <QRegularExpression>
#include <QScrollBar>
#include <QVBoxLayout>

#include <unordered_set>

IOptionWidget::IOptionWidget(QWidget* parent) : QWidget(parent)
{
    _outScrollLayout = new QHBoxLayout(this);
    _outScrollLayout->setSpacing(16);
    _outScrollLayout->setContentsMargins(0, 0, 4, 0);


    // _srollArea 
    _srollArea = new QScrollArea(this);
    _srollArea->setWidgetResizable(true);
    _srollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _srollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scrollContent = new QWidget(_srollArea);

    _wrapMainLayout = new QGridLayout(_scrollContent);
    _wrapMainLayout->setSpacing(0);
    _wrapMainLayout->setObjectName("_wrapMainLayout");
    _wrapMainLayout->setContentsMargins(0, 0, 0, 0);

    _mainLayout = new QGridLayout();
    _mainLayout->setSpacing(0);
    _mainLayout->setVerticalSpacing(35);
    _mainLayout->setObjectName("_mainLayout");
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    
    // _outScrollLayout -> _srollArea -> _scrollContent -> _wrapMainLayout -> _mainLayout -> innerWidgets
    _wrapMainLayout->addLayout(_mainLayout, 0, 0, 1, 1, Qt::AlignmentFlag::AlignTop);
    _scrollContent->setLayout(_wrapMainLayout);
    _scrollContent->show();
    _srollArea->setWidget(_scrollContent);

    _outScrollLayout->addWidget(_srollArea);


    // 기본 수직 스크롤바를 외부 스크롤바로 대체
    _outScrollBar = new QScrollBar(this);
    _outScrollBar->setOrientation(Qt::Orientation::Vertical);
    QSizePolicy scrollSizePolicy = _outScrollBar->sizePolicy();
    scrollSizePolicy.setRetainSizeWhenHidden(true);
    _outScrollBar->setSizePolicy(scrollSizePolicy);

    _outScrollLayout->addWidget(_outScrollBar);


    // 외부 스크롤바 -> 내부 스크롤바 제어
    connect(_outScrollBar, &QScrollBar::valueChanged, this, [this](const int value)
    {
        _srollArea->verticalScrollBar()->setValue(value);
    });
    // 내부 스크롤바 값 -> 외부 스크롤바에 반영
    connect(_srollArea->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](int, int)
    {
        syncInOutScrollbar();
    });
    connect(_srollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int)
    {
        syncInOutScrollbar();
    });
}

IOptionWidget::~IOptionWidget()
{
}

void IOptionWidget::initializeAfterCtor()
{
    const int contentMinWidth =
            _srollArea->widget()->sizeHint().width()
            + _srollArea->verticalScrollBar()->sizeHint().width()
            + _outScrollLayout->spacing()
            + _outScrollBar->sizeHint().width();

    setMinimumWidth(contentMinWidth);
}

void IOptionWidget::syncInOutScrollbar()
{
    const QScrollBar* scrollBar = _srollArea->verticalScrollBar();

    const int min = scrollBar->minimum();
    const int max = scrollBar->maximum();
    const int pageStep = scrollBar->pageStep();

    if (min == max)
    {
        _outScrollBar->hide();
    }
    else
    {
        _outScrollBar->setRange(min, max);
        _outScrollBar->setPageStep(pageStep);
        _outScrollBar->setValue(scrollBar->value());
        _outScrollBar->show();
    }
}

void IOptionWidget::apply()
{
}

void IOptionWidget::cancel()
{
}

void IOptionWidget::finish()
{
}

std::tuple<QGroupBox*, QVBoxLayout*> IOptionWidget::newOptionGroupBox(const QString& inGroupTitle, QGridLayout* inParentLayout
                                                                    , const int inRow, const int inColumn
                                                                    , Qt::Alignment inAlignment)
{
    auto [groupBox, vLayout] = generateGroupBox(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inAlignment | Qt::AlignTop);

    return {groupBox, vLayout};
}

std::tuple<QGroupBox*, QVBoxLayout*> IOptionWidget::newOptionGroupBox(const QString& inGroupTitle, QGridLayout* inParentLayout
                                                                          , const int inRow, const int inColumn
                                                                          , const int inRowSpan, const int inColumnSpan
                                                                          , Qt::Alignment inAlignment)
{
    auto [groupBox, vLayout] = generateGroupBox(inGroupTitle);

    inParentLayout->addWidget(groupBox, inRow, inColumn, inRowSpan, inColumnSpan, inAlignment | Qt::AlignTop);

    return {groupBox, vLayout};
}

std::tuple<QGroupBox*, QVBoxLayout*> IOptionWidget::addNewOptionGroupBox(const QString& inGroupTitle)
{
    return newOptionGroupBox(inGroupTitle, _mainLayout, _mainLayout->rowCount(), 0);
}

std::tuple<QGroupBox*, QVBoxLayout*> IOptionWidget::generateGroupBox(const QString& inGroupTitle)
{
    QString objStr = inGroupTitle;
    objStr.remove(QRegularExpression("\\s"));
    objStr.remove(QRegularExpression("[^a-zA-Z0-9_-]"));

    QGroupBox* groupBox = new QGroupBox(this);
    groupBox->setObjectName(objStr + "GroupBox");
    groupBox->setAlignment(Qt::AlignmentFlag::AlignLeading | Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
    groupBox->setFlat(true);
    groupBox->setTitle(inGroupTitle);

    QVBoxLayout* vLayout = new QVBoxLayout(groupBox);
    vLayout->setObjectName(objStr + "VLayout");
    vLayout->setContentsMargins(0, 0, 0, 0);

    return {groupBox, vLayout};
}

IOptionPage* IOptionWidget::getOptionPage() const
{
    if (_optionPage.isNull())
    {
        qDebug() << "option page is null";
    }

    return _optionPage.get();
}

void IOptionWidget::setOptionPage(IOptionPage* inOptionPage)
{
    _optionPage = inOptionPage;
}


// ~======================
// IOptionPage

template <class T>
size_t qHash(const QPointer<T> &ptr, size_t seed = 0)
{
    return qHash(ptr ? ptr.data() : 0, seed);
}

static QSet<QPointer<IOptionPage>>& optionsPages()
{
    static QSet<QPointer<IOptionPage>> staticOptionPages;

    return staticOptionPages;
}


IOptionPage::IOptionPage()
{
    optionsPages().insert(this);
}

IOptionPage::~IOptionPage()
{
    optionsPages().remove(this);
}

const QSet<QPointer<IOptionPage>>& IOptionPage::allOptionsPages()
{
    return optionsPages();
}

std::vector<IOptionPage*> IOptionPage::sortedOptionsPages()
{
    std::vector<IOptionPage*> sortedOptionPages;
    const QSet<QPointer<IOptionPage>>& optionPageSet = optionsPages();
    for (const auto& option : optionPageSet)
    {
        if (option)
        {
            sortedOptionPages.push_back(option);
        }
    }

    std::ranges::sort(sortedOptionPages, IOptionPage::compareOptionsPages);

    return sortedOptionPages;
}

bool IOptionPage::compareOptionsPages(const IOptionPage* inPage1, const IOptionPage* inPage2)
{
    return (inPage1->_priority == inPage2->_priority)
               ? (inPage1->getDisplayName() < inPage2->getDisplayName())
               : (inPage1->_priority < inPage2->_priority);
}

QString IOptionPage::getDisplayName() const
{
    return _displayName;
}

QString IOptionPage::getIconPath() const
{
    return _iconPath;
}

QIcon IOptionPage::getIcon() const
{
    return QIcon(_iconPath);
}

QWidget* IOptionPage::getOptionWidget()
{
    if (_optionWidget.isNull())
    {
        if (_optionWidgetCtor)
        {
            _optionWidget = _optionWidgetCtor();
            _optionWidget->setOptionPage(this);
        }
        else
        {
            qFatal() << "invalid _optionWidgetCtor" << objectName();
        }
    }
    return _optionWidget.data();
}

void IOptionPage::apply()
{
}

void IOptionPage::cancel()
{
}

void IOptionPage::finish()
{
    if (_optionWidget)
    {
        // 필수적이지 않습니다.
        // SettingsWidget이 _optionWidget의 소유권을 갖고, 수명을 관리하기 때문입니다.
        _optionWidget->deleteLater();
    }
}

void IOptionPage::setDisplayName(const QString& inDisplayName)
{
    _displayName = inDisplayName;
}

void IOptionPage::setIconPath(const QString& inIconPath)
{
    _iconPath = inIconPath;
}

void IOptionPage::setOptionWidgetCtor(const std::function<IOptionWidget*()>& inOptionWidgetCtor)
{
    _optionWidgetCtor = inOptionWidgetCtor;
}

void IOptionPage::setPriority(const OptionPriority inPriority)
{
    _priority = inPriority;
}
