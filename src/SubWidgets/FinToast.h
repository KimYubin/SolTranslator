// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTOAST_H
#define FINTOAST_H

#include <QObject>


class FinToast : public QObject
{
    Q_OBJECT

    explicit FinToast(QObject* parent = nullptr);
    ~FinToast() override;

public:
    /**
     * 토스트 메시지를 띄웁니다.
     * 부모 위젯이 있다면 부모 위젯에, 아니면 주화면 우측 하단에 표시됩니다. 
     * 
     * @param inMessage 메시지 내용 
     * @param inToastParent 메시지를 띄울 부모 위젯. 없다면 주 화면 우측 하단에 위치합니다.
     * @param inDuration 메시지가 보일 시간. 
     */
    static void showToast(const QString& inMessage, QWidget* inToastParent, const int inDuration = 1'500);
};


#endif //FINTOAST_H
