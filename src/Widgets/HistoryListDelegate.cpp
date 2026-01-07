// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryListDelegate.h"

#include <QApplication>
#include <QPainter>

#include <qevent.h>

#include "HistoryModel.h"
#include "SolLog.h"


constexpr int CheckBoxSize = 20;
constexpr int CheckBoxMargin = 5;

QRect checkBoxRect(const QStyleOptionViewItem& option)
{
    return QRect(option.rect.left() + CheckBoxMargin
               , option.rect.center().y() - (CheckBoxSize / 2)
               , CheckBoxSize
               , CheckBoxSize);
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

    const QStyle* appStyle = QApplication::style();

    // check box
    const bool bIsChecked = index.data(HistoryModel::CheckRole).toBool();
    opt.state.setFlag(bIsChecked ? QStyle::State_On : QStyle::State_Off);
    opt.features.setFlag(QStyleOptionViewItem::HasCheckIndicator);
    opt.checkState = bIsChecked ? Qt::Checked : Qt::Unchecked;

    // text
    opt.text = index.data(HistoryModel::TextRole).toString();
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    painter->restore();
}

bool HistoryListDelegate::editorEvent(QEvent* event
                                    , QAbstractItemModel* model
                                    , const QStyleOptionViewItem& option
                                    , const QModelIndex& index)
{
    auto checkToggle = [&]()
    {
        const bool checked = index.data(HistoryModel::CheckRole).toBool();
        model->setData(index, !checked, HistoryModel::CheckRole);
        return true;
    };

    switch (event->type())
    {
    case QEvent::MouseButtonRelease:
    {
        const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.features.setFlag(QStyleOptionViewItem::HasCheckIndicator);
        const QRect checkRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, opt.widget);

        if (checkRect.contains(mouseEvent->pos()))
        {
            return checkToggle();
        }
        break;
    }
    case QEvent::KeyRelease:
    {
        const QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key::Key_Space)
        {
            return checkToggle();
        }
        break;
    }
    default: break;
    }


    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
