// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryListDelegate.h"

#include <QApplication>
#include <QPainter>
#include <QTextLayout>

#include <qevent.h>
#include <qpainterstateguard.h>


#include "HistoryModel.h"
#include "HistoryWidget.h"
#include "SolGuard.h"
#include "SolLog.h"

namespace
{
constexpr int CheckBoxMargin = 5;

/** 유효한 option.widget이 있다면 widget의 style을 반환하고, 그렇지 않다면, QApplication::style()을 반환합니다. */
QStyle* getOptStyle(const QStyleOptionViewItem& inOpt)
{
    const QWidget* widget  = inOpt.widget;
    return widget ? widget->style() : QApplication::style();
}

QRect checkBoxRect(const QStyleOptionViewItem& inOpt)
{
    const QStyle* optStyle   = getOptStyle(inOpt);
    const QSize CheckboxSize = QSize(optStyle->pixelMetric(QStyle::PM_IndicatorWidth, &inOpt, inOpt.widget)
                                   , optStyle->pixelMetric(QStyle::PM_IndicatorHeight, &inOpt, inOpt.widget));

    return QRect(QPoint{inOpt.rect.left() + CheckBoxMargin, inOpt.rect.top() + CheckBoxMargin}
               , CheckboxSize);
}

} // anonymous namespace

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
    const QStyle* appStyle = getOptStyle(opt);
    opt.state.setFlag(true ? QStyle::State_On : QStyle::State_Off);
    opt.features.setFlag(QStyleOptionViewItem::HasCheckIndicator);
    opt.checkState = true ? Qt::Checked : Qt::Unchecked;
    
    // item
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    // checkbox
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(sol::CheckRole).toInt());

    QStyleOptionButton checkOpt;
    checkOpt.state = (checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;
    checkOpt.state.setFlag(QStyle::State_Enabled);
    checkOpt.rect = checkBoxRect(opt);
    checkOpt.rect = appStyle->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &checkOpt, widget);

    appStyle->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkOpt, painter, widget);

    // text
    const int textHeight   = option.fontMetrics.height();
    const int textMargin   = textHeight * 0;
    const int frameHMargin = appStyle->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, widget);

    // test
    {
        PainterPenStateGuard ppsg{painter};

        QRect itemViewTextRect = appStyle->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
        painter->setPen(QColor{0,255,0});
        painter->drawRect(itemViewTextRect);
        
        itemViewTextRect.setHeight(textHeight);
        painter->setPen(QColor{255,0,0});
        painter->drawRect(itemViewTextRect);
    }
    
    
    QRect itemViewTextRect = appStyle->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    itemViewTextRect.setHeight(textHeight);

    const QRect itemTextRect = itemViewTextRect;
    const QRect langTextRect   = itemTextRect.translated(checkOpt.rect.right() + checkOpt.rect.left(), 0);
    const QRect sourceTextRect = langTextRect.translated(0, textHeight + textMargin);
    const QRect targetTextRect = sourceTextRect.translated(0, textHeight + textMargin);

    const QString langText = index.data(sol::SourceLangRole).toString()
            + "->" + index.data(sol::TagetLangRole).toString()
            + "     " + index.data(sol::TimeStampRole).toString();

    {
        PainterFontStateGuard pfsg{painter};
        QFont newFont = painter->font();
        newFont.setPixelSize(painter->font().pixelSize() * 0.8);
        painter->setFont(newFont);
        drawText(painter, opt, langTextRect, langText);
    }
    drawText(painter, opt, sourceTextRect, index.data(sol::SourceTextRole).toString());
    drawText(painter, opt, targetTextRect, index.data(sol::TargetTextRole).toString());
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
        const QStyle* appStyle = getOptStyle(viewOpt);

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

QSize HistoryListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QWidget* widget  = option.widget;
    const QStyle* appStyle = getOptStyle(option);

    const int textHeight   = option.fontMetrics.height();
    const int textMargin   = textHeight * 0.2;
    const int frameHMargin = appStyle->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, widget);

    QSize superSize = QStyledItemDelegate::sizeHint(option, index);
    superSize.setHeight(textHeight * 3 + textMargin * 2 + frameHMargin * 2);

    return superSize;
}

void HistoryListDelegate::drawText(QPainter* painter
                                 , const QStyleOptionViewItem& inOption
                                 , const QRect& inTextRect
                                 , const QString& inText) const
{
    const HistoryListView* historyListView = qobject_cast<const HistoryListView*>(inOption.widget);

    if (historyListView == nullptr)
    {
        solDebug << "historyListView is not valid";
        return;
    }

    PainterPenStateGuard ppsg{painter};

    if (inOption.state.testFlag(QStyle::State_Selected))
    {
        painter->setPen(historyListView->getItemColor(sol::itemSelectionTextColorRole));
    }
    else if (inOption.state.testFlag(QStyle::State_MouseOver))
    {
        painter->setPen(historyListView->getItemColor(sol::itemHoverTextColorRole));
    }
    else
    {
        painter->setPen(historyListView->getItemColor(sol::itemTextColorRole));
    }

    if (inOption.state.testFlag(QStyle::State_Editing))
    {
        painter->setPen(historyListView->getItemColor(sol::itemTextColorRole));
    }

    const QStyle* appStyle = getOptStyle(inOption);

    appStyle->drawItemText(painter, inTextRect, Qt::TextForceLeftToRight, inOption.palette, true, inText, QPalette::NoRole);
}

