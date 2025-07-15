// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "IOptionWidget.h"

#include <QGroupBox>
#include <QRegularExpression>
#include <QVBoxLayout>

#include <unordered_set>

static std::unordered_set<IOptionPage*>& optionsPages()
{
    static std::unordered_set<IOptionPage*> staticOptionPages;
    return staticOptionPages;
}

IOptionWidget::IOptionWidget(QWidget* parent) : QWidget(parent)
{
}

IOptionWidget::~IOptionWidget()
{
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

IOptionPage::IOptionPage()
{
    optionsPages().emplace(this);
}

IOptionPage::~IOptionPage()
{
    optionsPages().erase(this);
}

const std::unordered_set<IOptionPage*>& IOptionPage::allOptionsPages()
{
    return optionsPages();
}

std::vector<IOptionPage*> IOptionPage::sortedOptionsPages()
{
    std::vector<IOptionPage*> sortedOptionPages(optionsPages().begin(), optionsPages().end());
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
    if (_optionWidget.isNull() == false)
    {
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
