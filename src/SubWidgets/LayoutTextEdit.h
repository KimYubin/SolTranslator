// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef LAYOUTTEXTEDIT_H
#define LAYOUTTEXTEDIT_H
#include "ResultTextEdit.h"


class SolButton;
class QPushButton;
class QHBoxLayout;

/**
 * 레이아웃이 포함된 텍스트 에디터입니다.
 * 오버레이 버튼 등을 추가할 수 있습니다.
 */
class LayoutTextEdit : public ResultTextEdit
{
    Q_OBJECT

public:
    LayoutTextEdit(QWidget* parent = nullptr);
    ~LayoutTextEdit() override;

    virtual void resizeEvent(QResizeEvent* event) override;

    /**
     * 텍스트 레이아웃 하단에 위젯을 추가합니다.
     */
    void addBottomWidget(QWidget* inWidget
                       , const int inStretch = 0
                       , const Qt::Alignment inAlignment = Qt::Alignment());

    /** add button at layout bottm */
    SolButton* addBottomButton(const QIcon& inIcon
                             , const Qt::FocusPolicy policy
                             , const QString& inToolTip
                             , const QKeySequence& inKey       = QKeySequence()
                             , const int inStretch             = 0
                             , const Qt::Alignment inAlignment = Qt::Alignment());

protected:
    QWidget* _bottomLayoutWidget;
    QHBoxLayout* _layout;
};


#endif //LAYOUTTEXTEDIT_H
