// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryListDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QTextLayout>

#include <qevent.h>
#include <qpainterstateguard.h>


#include "HistoryModel.h"
#include "HistoryWidget.h"
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

    QPainterStateGuard psg(painter);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QWidget* widget  = opt.widget;
    const QStyle* appStyle = widget ? widget->style() : QApplication::style();

    int align = Qt::AlignLeft; //QStyle::visualAlignment(Qt::AlignLeft, QFlag(d->align));

    // text
    opt.text = index.data(sol::TargetTextRole).toString();
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    
    // int flags = align | (d->textDirection() == Qt::LeftToRight ? Qt::TextForceLeftToRight: Qt::TextForceRightToLeft);

    QRect textRect = appStyle->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);

    drawText(painter, opt, textRect, index.data(sol::TargetTextRole).toString());


    // check
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(sol::CheckRole).toInt());

    QStyleOptionButton checkOpt;
    checkOpt.state = (checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;
    checkOpt.state.setFlag(QStyle::State_Enabled);
    checkOpt.rect = checkBoxRect(opt);
    checkOpt.rect = appStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &checkOpt, widget);

    appStyle->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkOpt, painter, widget);

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

static QSizeF viewItemTextLayout(QTextLayout& textLayout
                               , const int lineWidth)
{
    qreal height    = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    int i = 0;
    while (true)
    {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height    += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        // we assume that the height of the next line is the same as the current one
        ++i;
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

void HistoryListDelegate::drawText(QPainter* painter
                                 , const QStyleOptionViewItem& inOption
                                 , const QRect& inTextRect
                                 , const QString& inText) const
{
    QStyleOptionViewItem opt{inOption};
    QPalette::ColorGroup cg = opt.state.testFlag(QStyle::State_Enabled)
                                  ? QPalette::Normal
                                  : QPalette::Disabled;

    const QWidget* widget  = opt.widget;
    const QStyle* appStyle = widget ? widget->style() : QApplication::style();
    const HistoryListView* historyListView = qobject_cast<HistoryListView*>(const_cast<QWidget*>(opt.widget));
    
    if (historyListView == nullptr)
    {
        solDebug<<"historyListView is not valid";
        return;
    }

    // painter->drawText
    if (opt.state.testFlag(QStyle::State_Selected))
    {
        painter->setPen(historyListView->getItemColor(sol::itemSelectionTextColorRole));
    }
    else if (opt.state.testFlag(QStyle::State_MouseOver))
    {
        painter->setPen(historyListView->getItemColor(sol::itemHoverTextColorRole));
    }
    else
    {
        painter->setPen(historyListView->getItemColor(sol::itemTextColorRole));
    }

    if (opt.state.testFlag(QStyle::State_Editing))
    {
        painter->setPen(historyListView->getItemColor(sol::itemTextColorRole));
    }

    QRect newrec = inTextRect.translated(15,0);

    painter->drawText(newrec, Qt::TextForceLeftToRight, inText);

    // appStyle->drawItemText

    newrec = newrec.translated(0,12);
    
    appStyle->drawItemText(painter, newrec, Qt::TextForceLeftToRight, opt.palette, true, inText, QPalette::NoRole);
}

