//
// Created by YubinKim on 25/04/14 월.
//

#include "IOptionWidget.h"

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

IOptionPage::IOptionPage()
{
    optionsPages().emplace(this);
}

IOptionPage::~IOptionPage()
{
    optionsPages().erase(this);
}

const std::unordered_set<IOptionPage*> IOptionPage::allOptionsPages()
{
    return optionsPages();
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
