// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_TOOLTIPDATA_HPP
#define SOLTRANSLATOR_TOOLTIPDATA_HPP

#include <QKeySequence>
#include <QMetaType>
#include <QString>

struct ToolTipData
{
    static const char* Name;

    ToolTipData() = default;

    ToolTipData(QString inToolTip
              , std::optional<QString> inOffToolTip
              , QKeySequence inShortcut
              , const bool inIsOnToolTip = true)
        : toolTip(std::move(inToolTip))
        , OffToolTip(std::move(inOffToolTip))
        , shortcut(std::move(inShortcut))
        , isOnToolTip(inIsOnToolTip)
    {}

    ToolTipData(QString inToolTip
              , const QKeySequence& inShortcut)
        : toolTip(std::move(inToolTip))
        , OffToolTip(std::nullopt)
        , shortcut(std::move(inShortcut.toString()))
    {}

    explicit ToolTipData(QString inToolTip)
        : toolTip(std::move(inToolTip))
        , OffToolTip(std::nullopt)
        , shortcut(QKeySequence{})
    {}

    explicit ToolTipData(const QKeySequence& inShortcut)
        : toolTip(QString{})
        , OffToolTip(std::nullopt)
        , shortcut(std::move(inShortcut.toString()))
    {}

    ToolTipData(const ToolTipData&)             = default;
    ToolTipData(ToolTipData&& inOther) noexcept = default;

    ToolTipData& operator=(const ToolTipData&)             = default;
    ToolTipData& operator=(ToolTipData&& inOther) noexcept = default;

    ~ToolTipData() = default;

    QString toolTipString() const;
    QString toolTipShortcutString() const;

    QString toolTip;
    std::optional<QString> OffToolTip;
    QKeySequence shortcut;
    bool isOnToolTip = true;
    bool isVisible   = true;
};


Q_DECLARE_METATYPE(ToolTipData);

#endif //SOLTRANSLATOR_TOOLTIPDATA_HPP
