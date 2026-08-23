// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "IOptionWidget.h"

#include "SettingWidgetFactory.h"
#include "SubWidgets/SolSmoothScrollBar.h"
#include "Utils/SolDebug.h"

#include <QGroupBox>
#include <QRegularExpression>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>


namespace Sol
{
IOptionWidget::IOptionWidget(QWidget* inParent) : QWidget(inParent)
{
    _outScrollLayout = new QHBoxLayout(this);
    _outScrollLayout->setSpacing(16);
    _outScrollLayout->setContentsMargins(0, 0, 4, 0);


    // _srollArea
    _srollArea = new SolSmoothScrollArea(this);
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
    _outScrollBar = new SolSmoothScrollBar(this);
    _outScrollBar->setOrientation(Qt::Orientation::Vertical);
    QSizePolicy scrollSizePolicy = _outScrollBar->sizePolicy();
    scrollSizePolicy.setRetainSizeWhenHidden(true);
    _outScrollBar->setSizePolicy(scrollSizePolicy);
    _outScrollBar->setSingleStep(_srollArea->verticalScrollBar()->singleStep());
    _outScrollLayout->addWidget(_outScrollBar);


    // 외부 스크롤바 -> 내부 스크롤바 제어
    connect(_outScrollBar, &QScrollBar::valueChanged, this, [this](const int inValue)
    {
        _srollArea->verticalScrollBar()->setValue(inValue);
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
{}

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
{}

void IOptionWidget::cancel()
{}

void IOptionWidget::finish()
{}


OptionGroupBox* IOptionWidget::addNewOptionGroupBox(const QString& inGroupTitle)
{
    return OptionWidgetFactory::createOptionGroupBox(inGroupTitle, _mainLayout, _mainLayout->rowCount(), 0);
}

IOptionPage* IOptionWidget::getOptionPage() const
{
    if (_optionPage.isNull())
    {
        solDebug << "option page is null";
    }

    return _optionPage.get();
}

void IOptionWidget::setOptionPage(IOptionPage* inOptionPage)
{
    _optionPage = inOptionPage;
}


// ~======================
// IOptionPage

namespace
{
template <class T>
struct QPointer_hasher
{
    size_t operator()(const QPointer<T>& ptr, size_t seed = 0) const
    {
        return qHash(ptr ? ptr.data() : 0, seed);
    }
};

template <class T>
using unorder_set_qpointer = std::unordered_set<QPointer<T>, QPointer_hasher<T>>;

unorder_set_qpointer<IOptionPage>& optionsPages()
{
    static unorder_set_qpointer<IOptionPage> staticOptionPages;

    return staticOptionPages;
}
} // anonymous namespace

IOptionPage::IOptionPage()
    : _priority(std::numeric_limits<int>::max())
{
    optionsPages().insert(QPointer{this});
}

IOptionPage::~IOptionPage()
{
    optionsPages().erase(QPointer{this});
}

void IOptionPage::allOptionsFinish()
{
    unorder_set_qpointer<IOptionPage>& options = optionsPages();
    for (const QPointer<IOptionPage>& option : options)
    {
        if (option)
        {
            option->finish();
        }
    }
}

std::vector<QPointer<IOptionPage>> IOptionPage::sortedOptionsPages()
{
    unorder_set_qpointer<IOptionPage>& optionSet = optionsPages();

    // cleanup nullptr
    std::erase_if(optionSet, [](const QPointer<IOptionPage>& inVal)
    {
        return inVal.isNull();
    });

    std::vector<QPointer<IOptionPage>> resVec;
    resVec.reserve(optionSet.size());

    std::ranges::copy(optionSet, std::back_inserter(resVec));
    std::ranges::sort(resVec, {}, [](const QPointer<IOptionPage>& inVal)
    {
        return std::tie(inVal->_priority, inVal->_displayName);
    });

    return resVec;
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
{}

void IOptionPage::cancel()
{}

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

void IOptionPage::setOptionWidgetCtor(Callback<IOptionWidget*(void)>&& inOptionWidgetCtor)
{
    _optionWidgetCtor = std::move(inOptionWidgetCtor);
}

void IOptionPage::setPriority(const int inPriority)
{
    _priority = inPriority;
}
} // namespace Sol
