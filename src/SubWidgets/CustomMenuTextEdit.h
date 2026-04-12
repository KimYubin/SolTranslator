// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef CUSTOMMENUTEXTEDIT_H
#define CUSTOMMENUTEXTEDIT_H

#include <QPlainTextEdit>
#include <QTextBrowser>


/**
 * contextMenu의 창테두리와 배경을 투명하게 처리하기 위한 클래스입니다. 
 */
class MenuPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit MenuPlainTextEdit(QWidget* parent = nullptr);
    ~MenuPlainTextEdit() override;

    virtual void contextMenuEvent(QContextMenuEvent* event) override;
};


class MenuTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit MenuTextEdit(QWidget* parent = nullptr);
    ~MenuTextEdit() override;

    virtual void contextMenuEvent(QContextMenuEvent* event) override;
};


class MenuTextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit MenuTextBrowser(QWidget* parent = nullptr);
    ~MenuTextBrowser() override;

    virtual void contextMenuEvent(QContextMenuEvent* event) override;
};


#endif //CUSTOMMENUTEXTEDIT_H
