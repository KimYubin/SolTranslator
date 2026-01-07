// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryListDelegate.h"

#include <QApplication>
#include <QPainter>

#include <qevent.h>

#include "HistoryModel.h"
#include "SolLog.h"


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

    const QStyle* appStyle = QApplication::style();

    // check box
    opt.features.setFlag(QStyleOptionViewItem::HasCheckIndicator);
    opt.checkState = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());

    // text
    opt.text = index.data(Qt::DisplayRole).toString();
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    painter->restore();
}

bool HistoryListDelegate::editorEvent(QEvent* event
                                    , QAbstractItemModel* model
                                    , const QStyleOptionViewItem& option
                                    , const QModelIndex& index)
{

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
