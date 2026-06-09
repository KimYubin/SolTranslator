// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_TOOLTIPDATA_HPP
#define SOLTRANSLATOR_TOOLTIPDATA_HPP

#include <QKeySequence>
#include <QString>

struct ToolTipData
{
    inline static const char* Name = "SolToolTipData";

    ToolTipData() = default;

    ToolTipData(QString inToolTip
              , std::optional<QString> inOffToolTip
              , QString inShortcutString)
        : toolTip(std::move(inToolTip))
        , OffToolTip(std::move(inOffToolTip))
        , shortcut(std::move(inShortcutString))
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

    ~ToolTipData()                             = default;
    ToolTipData(const ToolTipData&)            = default;
    ToolTipData& operator=(const ToolTipData&) = default;

    QString getToolTip();

    QString toolTip;
    std::optional<QString> OffToolTip;
    QKeySequence shortcut;
    bool isOn;
};

Q_DECLARE_METATYPE(ToolTipData);

#endif //SOLTRANSLATOR_TOOLTIPDATA_HPP
