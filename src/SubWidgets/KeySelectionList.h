// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef KEYSELECTIONLIST_H
#define KEYSELECTIONLIST_H
#include <QListWidget>

/** 키보드로 아이템 선택이 가능한 리스트 위젯입니다. */
class KeySelectionList : public QListWidget
{
    Q_OBJECT

public:
    explicit KeySelectionList(QWidget* parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

protected: signals:
    void itemKeyPressed(QListWidgetItem* item);
    
};


#endif //KEYSELECTIONLIST_H
