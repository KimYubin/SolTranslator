// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolToolTip.h"

#include "SolToolTipBallon.h"
#include "Managers/ConfigManager.h"
#include "Types/ToolTipData.hpp"
#include "Utils/SolLog.h"

#include <QAbstractButton>
#include <QEvent>


namespace
{

std::unordered_map<QWidget*, ToolTipData>& toolTipDatas()
{
    static std::unordered_map<QWidget*, ToolTipData> shortcutMap;
    return shortcutMap;
}

void removeToolTipData(QWidget* inWidget)
{
    toolTipDatas().erase(inWidget);
}

void removeToolTipDataByObj(QObject* inObj)
{
    if (QWidget* inWidget = qobject_cast<QWidget*>(inObj))
    {
        toolTipDatas().erase(inWidget);
    }
}

void setToolTipData(QWidget* inWidget, const ToolTipData& inToolTipData)
{
    if (toolTipDatas().contains(inWidget) == false)
    {
        QObject::connect(inWidget, &QObject::destroyed, &removeToolTipDataByObj);
    }

    toolTipDatas()[inWidget] = inToolTipData;
}


std::tuple<bool, ToolTipData*> findToolTipData(QWidget* inWidget)
{
    const auto findIt = toolTipDatas().find(inWidget);
    if (findIt == toolTipDatas().end())
    {
        return {false, nullptr};
    }

    return {true, &findIt->second};
}


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

void SolToolTip::setToolTip(QWidget* inWidget, const QString& inToolTip)
{
    static SolToolTipFilter* toolTipEventFilter = new SolToolTipFilter();
    inWidget->setToolTip(inToolTip);
    inWidget->installEventFilter(toolTipEventFilter);

    inWidget->setProperty(ToolTipData::Name, QVariant::fromValue(ToolTipData{inToolTip}));
    solDebug << inWidget->dynamicPropertyNames();
}

void SolToolTip::setToolTipShortcut(QWidget* inWidget
                                  , const QString& inToolTip
                                  , const QKeySequence& inKey)
{
    inWidget->setProperty(ToolTipData::Name, QVariant::fromValue(ToolTipData{inToolTip, inKey}));

    setToolTipData(inWidget, {inToolTip, inKey});
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
    ToolTipData& toolTipData = toolTipDatas()[inWidget];
    toolTipData.shortcut = inKey;
}

void SolToolTip::setAction(QWidget* inWidget
                         , const Action inAction)
{
    changeShortcut(inWidget, solConfig.shortcut(inAction));
}


void SolToolTip::setCheckButtonToolTip(QAbstractButton* inButton
                                     , const QString& inOnToolTip
                                     , const QString& inOffToolTip)
{
    const bool isChecked = (inButton->isCheckable() && inButton->isChecked());

    setToolTip(inButton, isChecked ? inOnToolTip : inOffToolTip);

    connect(inButton, &QAbstractButton::toggled, inButton, [inButton, inOnToolTip, inOffToolTip](const bool checked)
    {
        const QString& toolTip = checked ? inOnToolTip : inOffToolTip;
        inButton->setToolTip(toolTip);

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
        auto [isFind, tooltipDataPtr] = findToolTipData(widget);
        // if (isFind)
        {
            SolToolTipBallon::instance()->showToolTip(widget, tooltipDataPtr);
        }

        if (widget->toolTip().isEmpty() == false)
        {
            // SolToolTipBallon::instance()->showToolTip(widget);
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
