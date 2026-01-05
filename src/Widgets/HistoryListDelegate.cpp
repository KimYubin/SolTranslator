// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryListDelegate.h"

#include <QApplication>
#include <QPainter>

#include "HistoryModel.h"


constexpr int CheckBoxSize = 20;
constexpr int Margin = 5;

QRect checkBoxRect(const QStyleOptionViewItem &option)
{
    return QRect(option.rect.left() + Margin,
                 option.rect.center().y() - CheckBoxSize / 2,
                 CheckBoxSize,
                 CheckBoxSize);
}

void HistoryListDelegate::paint(QPainter* painter
                              , const QStyleOptionViewItem& option
                              , const QModelIndex& index) const
{
    if (index.isValid() == false)
    {
        return;
    }

    painter->save();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // check box
    const bool bIsChecked = index.data(HistoryModel::CheckRole).toBool();

    QStyleOptionButton checkOpt;
    checkOpt.rect  = checkBoxRect(opt);
    checkOpt.state = bIsChecked ? QStyle::State_On : QStyle::State_Off;
    checkOpt.state |= QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkOpt, painter, opt.widget);

    // text
    opt.text = index.data(HistoryModel::TextRole).toString();
    opt.rect.adjust(CheckBoxSize + 2 * Margin, 0, 0, 0);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    painter->restore();
}

bool HistoryListDelegate::editorEvent(QEvent* event
                                    , QAbstractItemModel* model
                                    , const QStyleOptionViewItem& option
                                    , const QModelIndex& index)
{
    
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
