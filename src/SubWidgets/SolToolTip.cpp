// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolToolTip.h"

#include "SolToolTipBallon.h"
#include "Managers/ConfigManager.h"
#include "Types/ToolTipData.h"
#include "Utils/SolLog.h"

#include <QAbstractButton>
#include <QEvent>


namespace
{
// ~==================================
// SolToolTipFilter
class SolToolTipFilter : public QObject
{
    Q_OBJECT

public:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;
};

#include "SolToolTip.moc"
} // anonymous namespace


// ~==================================
// SolToolTip

SolToolTip::SolToolTip(QObject* parent) : QObject(parent)
{}

bool SolToolTip::isValidToolTip(const QWidget* inWidget)
{
    return inWidget->property(ToolTipData::Name).isValid();
}

ToolTipData SolToolTip::getToolTipData(const QWidget* inWidget)
{
    return inWidget->property(ToolTipData::Name).value<ToolTipData>();
}

void SolToolTip::setToolTipProperty(QWidget* inWidget, ToolTipData inToolTipData)
{
    static SolToolTipFilter toolTipEventFilter;

    // For accessible Description.
    inWidget->setToolTip(inToolTipData.toolTipString());
    inWidget->setProperty(ToolTipData::Name, QVariant::fromValue(std::move(inToolTipData)));
    inWidget->installEventFilter(&toolTipEventFilter);
}

void SolToolTip::setToolTip(QWidget* inWidget, const QString& inToolTip)
{
    setToolTipProperty(inWidget, ToolTipData{inToolTip});
}

void SolToolTip::setToolTipShortcut(QWidget* inWidget
                                  , const QString& inToolTip
                                  , const QKeySequence& inKey)
{
    setToolTipProperty(inWidget, ToolTipData{inToolTip, inKey});
}

void SolToolTip::setToolTipAction(QWidget* inWidget
                                , const QString& inToolTip
                                , const Action inAction)
{
    setToolTipShortcut(inWidget, inToolTip, solConfig.shortcut(inAction));
}

void SolToolTip::changeShortcut(QWidget* inWidget
                              , const QKeySequence& inKey)
{
    ToolTipData toolTipData = getToolTipData(inWidget);
    toolTipData.shortcut    = inKey;
    setToolTipProperty(inWidget, std::move(toolTipData));
}

void SolToolTip::setAction(QWidget* inWidget
                         , const Action inAction)
{
    changeShortcut(inWidget, solConfig.shortcut(inAction));
}


void SolToolTip::setCheckButtonToolTip(QAbstractButton* inButton
                                     , const QString& inOnToolTip
                                     , const QString& inOffToolTip
                                     , const QKeySequence& inKey)
{
    const bool isChecked = (inButton->isCheckable() && inButton->isChecked());

    setToolTipProperty(inButton, {inOnToolTip, inOffToolTip, inKey, isChecked});

    connect(inButton, &QAbstractButton::toggled, inButton, [inButton](const bool checked)
    {
        ToolTipData toolTipData = inButton->property(ToolTipData::Name).value<ToolTipData>();
        toolTipData.isOnToolTip = checked;
        setToolTipProperty(inButton, std::move(toolTipData));

        SolToolTipBallon::instance()->updateWidgetToolTip(inButton);
    });
}

bool SolToolTipFilter::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::ToolTip:
    {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget == nullptr)
        {
            return false;
        }
        
        if (SolToolTip::isValidToolTip(widget))
        {
            SolToolTipBallon::instance()->showToolTip(widget);
        }

        return true; // 기본 툴팁 차단
    }
    case QEvent::Leave:
    case QEvent::Hide:
    case QEvent::Close:
    case QEvent::Quit:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
    {
        SolToolTipBallon::instance()->hideToolTipImmediately();
        break;
    }
    default: break;
    }

    return QObject::eventFilter(obj, event);
}
