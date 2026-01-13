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
               , option.rect.top() + CheckBoxMargin
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

    const QWidget* widget  = opt.widget;
    const QStyle* appStyle = widget ? widget->style() : QApplication::style();

    int align = Qt::AlignLeft; //QStyle::visualAlignment(Qt::AlignLeft, QFlag(d->align));

    // text
    opt.text = index.data(sol::TargetTextRole).toString();
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    // int flags = align | (d->textDirection() == Qt::LeftToRight ? Qt::TextForceLeftToRight                                                           : Qt::TextForceRightToLeft);
    opt.rect = appStyle->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);

    QPalette::ColorRole textColorRole = QPalette::NoRole;
    if (opt.state & QStyle::State_Selected)
    {
        textColorRole = QPalette::HighlightedText;
    }
    else
    {
        textColorRole = QPalette::Text;
    }
    if (opt.state & QStyle::State_Editing)
    {
        textColorRole = QPalette::Text;
    }


    appStyle->drawItemText(painter, opt.rect, Qt::TextForceLeftToRight, opt.palette, true, opt.text, textColorRole);


    // check
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(sol::CheckRole).toInt());

    QStyleOptionButton checkOpt;
    checkOpt.state = (checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;
    checkOpt.state.setFlag(QStyle::State_Enabled);
    checkOpt.rect = checkBoxRect(opt);
    checkOpt.rect = appStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &checkOpt, widget);

    appStyle->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkOpt, painter, widget);

    painter->restore();
}

bool HistoryListDelegate::editorEvent(QEvent* event
                                    , QAbstractItemModel* model
                                    , const QStyleOptionViewItem& option
                                    , const QModelIndex& index)
{
    const Qt::ItemFlags flags = model->flags(index);
    if (flags.testFlag(Qt::ItemIsUserCheckable) == false
        || option.state.testFlag(QStyle::State_Enabled) == false
        || flags.testFlag(Qt::ItemIsEnabled) == false)
    {
        return false;
    }

    const QVariant value = index.data(sol::CheckRole);
    if (value.isValid() == false)
    {
        return false;
    }

    const QEvent::Type eventType = event->type();

    if ((eventType == QEvent::MouseButtonRelease)
        || (eventType == QEvent::MouseButtonDblClick)
        || (eventType == QEvent::MouseButtonPress))
    {
        QStyleOptionViewItem viewOpt(option);
        initStyleOption(&viewOpt, index);
        const QWidget* widget  = option.widget;
        const QStyle* appStyle = widget ? widget->style() : QApplication::style();


        QStyleOptionButton checkOpt;
        checkOpt.rect = checkBoxRect(viewOpt);
        checkOpt.rect = appStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &checkOpt, widget);

        const QRect checkRect = appStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &checkOpt, widget);

        const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() != Qt::LeftButton
            || (checkRect.contains(mouseEvent->position().toPoint()) == false))
        {
            return false;
        }
        if ((eventType == QEvent::MouseButtonPress)
            || (eventType == QEvent::MouseButtonDblClick))
        {
            return true;
        }
    }
    else if (eventType == QEvent::KeyPress)
    {
        switch (static_cast<QKeyEvent*>(event)->key())
        {
        case Qt::Key_Space:
        case Qt::Key_Select:
        case Qt::Key_Enter: // QAbstractItemView::keyPressEvent에서 ignore처리됨. 수정필요.
        case Qt::Key_Return:
            break;
        default:
            return false;
        }
    }
    else
    {
        return false;
    }

    Qt::CheckState state = static_cast<Qt::CheckState>(index.data(sol::CheckRole).toInt());
    if (flags.testFlag(Qt::ItemIsUserTristate))
    {
        state = static_cast<Qt::CheckState>((state + 1) % 3);
    }
    else
    {
        state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    }

    return model->setData(index, state, sol::CheckRole);
}

}
