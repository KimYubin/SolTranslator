// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryListDelegate.h"

#include "HistoryListView.h"
#include "HistoryModel.h"
#include "HistoryWidget.h"
#include "SolGuard.h"
#include "SolUtilibrary.h"
#include "Utils/SolLog.h"

#include <QApplication>
#include <QPainter>
#include <QTextLayout>
#include <qevent.h>
#include <qpainterstateguard.h>

namespace
{
constexpr float langFontSizeRatio   = 0.9f;
constexpr float textVMarginRatio    = 0.1f;
constexpr int checkBoxToTextSpacing = 5;
constexpr QPoint focusInnerPadding{3, 3};

/** 유효한 option.widget이 있다면 widget의 style을 반환하고, 그렇지 않다면, QApplication::style()을 반환합니다. */
QStyle* getOptStyle(const QStyleOptionViewItem& inOpt)
{
    const QWidget* widget  = inOpt.widget;
    return widget ? widget->style() : QApplication::style();
}

QSize getCheckBoxSize(const QStyleOptionViewItem& inOpt)
{
    const QStyle* optStyle = getOptStyle(inOpt);
    return QSize(optStyle->pixelMetric(QStyle::PM_IndicatorWidth, &inOpt, inOpt.widget)
               , optStyle->pixelMetric(QStyle::PM_IndicatorHeight, &inOpt, inOpt.widget));
}

QMargins getFocusMargins(const QStyleOptionViewItem& inOpt)
{
    const QStyle* optStyle    = getOptStyle(inOpt);
    const QRect itemFocusRect = optStyle->subElementRect(QStyle::SE_ItemViewItemFocusRect, &inOpt, inOpt.widget);

    return {
        itemFocusRect.left() - inOpt.rect.left()
      , itemFocusRect.top() - inOpt.rect.top()
      , inOpt.rect.right() - itemFocusRect.right()
      , inOpt.rect.bottom() - itemFocusRect.bottom()
    };
}

QRect checkBoxRect(const QStyleOptionViewItem& inOpt)
{
    const QStyle* optStyle    = getOptStyle(inOpt);
    const QSize checkboxSize  = getCheckBoxSize(inOpt);
    const QRect itemFocusRect = optStyle->subElementRect(QStyle::SE_ItemViewItemFocusRect, &inOpt, inOpt.widget);

    return QRect(itemFocusRect.topLeft() + focusInnerPadding
               , checkboxSize);
}

} // anonymous namespace

using Sol::HistoryRole;

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

    // item
    appStyle->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    // checkbox
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(HistoryRole::CheckRole).toInt());

    QStyleOptionButton checkOpt;
    checkOpt.state = (checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;
    checkOpt.state.setFlag(QStyle::State_Enabled);
    checkOpt.rect = checkBoxRect(opt);

    appStyle->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkOpt, painter, widget);

    // calculate text rect
    const int textHeight     = option.fontMetrics.height() * (1.0f + textVMarginRatio);
    const int langTextHeight = textHeight * langFontSizeRatio;

    const int textLeft       = checkOpt.rect.right() + checkBoxToTextSpacing; // checkbox area. checkbox right + checkbox space
    const QRect itemTextRect = appStyle->subElementRect(QStyle::SE_ItemViewItemText, &opt, widget);
    constexpr int focusMargin = 2;

    const QPoint textTopLeft = itemTextRect.topLeft() + QPoint{textLeft, 0};
    const int textWidth      = itemTextRect.width() - textLeft - focusMargin;

    const QRect langTextRect   = QRect{textTopLeft, QSize{textWidth, langTextHeight}};
    const QRect sourceTextRect = QRect{langTextRect.bottomLeft(), QSize{textWidth, textHeight}};
    const QRect targetTextRect = QRect{sourceTextRect.bottomLeft(), QSize{textWidth, textHeight}};

    // language text & time stamp
    {
        PainterFontStateGuard pfsg{painter};

        QFont newFont = painter->font();
        newFont.setPixelSize(painter->font().pixelSize() * langFontSizeRatio);
        painter->setFont(newFont);

        const QString langText = index.data(HistoryRole::SourceLangRole).toString() + " → " + index.data(HistoryRole::TagetLangRole).toString();

        drawText(painter, opt, langTextRect, Qt::TextForceLeftToRight | Qt::AlignLeft, langText);
        drawText(painter, opt, langTextRect, Qt::AlignRight, index.data(HistoryRole::TimeStampRole).toString());
    }
    drawText(painter, opt, sourceTextRect, Qt::TextForceLeftToRight, index.data(HistoryRole::SourceSimplifiedTextRole).toString());
    drawText(painter, opt, targetTextRect, Qt::TextForceLeftToRight, index.data(HistoryRole::TargetSimplifiedTextRole).toString());
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

    const QVariant value = index.data(HistoryRole::CheckRole);
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

        const QRect checkRect = checkBoxRect(viewOpt);

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

    Qt::CheckState state = static_cast<Qt::CheckState>(index.data(HistoryRole::CheckRole).toInt());
    if (flags.testFlag(Qt::ItemIsUserTristate))
    {
        state = static_cast<Qt::CheckState>((state + 1) % 3);
    }
    else
    {
        state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    }

    return model->setData(index, state, HistoryRole::CheckRole);
}

QSize HistoryListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const int textHeight        = option.fontMetrics.height();
    const int textMargin        = textHeight * textVMarginRatio;
    const QMargins focusMargins = getFocusMargins(option);
    const int frameVMargin      = focusMargins.top() + focusMargins.bottom();

    QSize sizeHint = QStyledItemDelegate::sizeHint(option, index);
    sizeHint.setHeight(textHeight * (2 + langFontSizeRatio) + textMargin * 2 + frameVMargin);

    return sizeHint;
}

void HistoryListDelegate::drawText(QPainter* painter
                                 , const QStyleOptionViewItem& inOption
                                 , const QRect& inTextRect
                                 , const int flags
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
        painter->setPen(historyListView->getItemColor(Sol::itemSelectionTextColorRole));
    }
    else if (inOption.state.testFlag(QStyle::State_MouseOver))
    {
        painter->setPen(historyListView->getItemColor(Sol::itemHoverTextColorRole));
    }
    else
    {
        painter->setPen(historyListView->getItemColor(Sol::itemTextColorRole));
    }

    if (inOption.state.testFlag(QStyle::State_Editing))
    {
        painter->setPen(historyListView->getItemColor(Sol::itemTextColorRole));
    }

    const QStyle* appStyle = getOptStyle(inOption);

    appStyle->drawItemText(painter, inTextRect, flags, inOption.palette, true, inText, QPalette::NoRole);
}

