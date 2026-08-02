// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef KEYSELECTIONLIST_H
#define KEYSELECTIONLIST_H
#include <QListWidget>

/** 키보드로 아이템 선택이 가능한 리스트 위젯입니다. */
class KeySelectionList : public QListWidget
{
    Q_OBJECT

public:
    explicit KeySelectionList(QWidget* inParent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *inEvent) override;

signals:
    void itemKeyPressed(QListWidgetItem* inItem);

};


#endif //KEYSELECTIONLIST_H
