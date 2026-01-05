// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_HISTORYLISTDELEGATE_H
#define SOLTRANSLATOR_HISTORYLISTDELEGATE_H
#include <QStyledItemDelegate>


class HistoryListDelegate : public QStyledItemDelegate
{
public:
    virtual void paint(QPainter* painter
                     , const QStyleOptionViewItem& option
                     , const QModelIndex& index) const override;

    virtual bool editorEvent(QEvent* event
                           , QAbstractItemModel* model
                           , const QStyleOptionViewItem& option
                           , const QModelIndex& index) override;
};

#endif //SOLTRANSLATOR_HISTORYLISTDELEGATE_H
