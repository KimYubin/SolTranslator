// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYLISTDELEGATE_H
#define SOLTRANSLATOR_HISTORYLISTDELEGATE_H

#include <QStyledItemDelegate>

class HistoryListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    virtual void paint(QPainter* painter
                     , const QStyleOptionViewItem& option
                     , const QModelIndex& index) const override;

    virtual bool editorEvent(QEvent* event
                           , QAbstractItemModel* model
                           , const QStyleOptionViewItem& option
                           , const QModelIndex& index) override;

    virtual QSize sizeHint(const QStyleOptionViewItem& option
                         , const QModelIndex& index) const override;

private:
    void drawText(QPainter* painter
                , const QStyleOptionViewItem& inOption
                , const QRect& inTextRect
                , const int flags
                , const QString& inText) const;
};

#endif //SOLTRANSLATOR_HISTORYLISTDELEGATE_H
