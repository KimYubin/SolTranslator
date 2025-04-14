//
// Created by YubinKim on 25/04/14 월.
//

#include "IOptionWidget.h"

static QList<IOptionPage*>& optionsPages()
{
    static QList<IOptionPage*> staticOptionPages;
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
    optionsPages().append(this);
}

IOptionPage::~IOptionPage()
{
    optionsPages().removeOne(this);
}

const QList<IOptionPage*> IOptionPage::allOptionsPages()
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
