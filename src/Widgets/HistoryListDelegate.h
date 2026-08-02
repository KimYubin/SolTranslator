// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYLISTDELEGATE_H
#define SOLTRANSLATOR_HISTORYLISTDELEGATE_H

#include <QStyledItemDelegate>

class HistoryListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    void paint(QPainter* inPainter
             , const QStyleOptionViewItem& inOption
             , const QModelIndex& inIndex) const override;


    QSize sizeHint(const QStyleOptionViewItem& inOption
                 , const QModelIndex& inIndex) const override;

protected:
    bool editorEvent(QEvent* inEvent
                   , QAbstractItemModel* inModel
                   , const QStyleOptionViewItem& inOption
                   , const QModelIndex& inIndex) override;

private:
    void drawText(QPainter* inPainter
                , const QStyleOptionViewItem& inOption
                , const QRect& inTextRect
                , const int inFlags
                , const QString& inText) const;
};

#endif //SOLTRANSLATOR_HISTORYLISTDELEGATE_H
