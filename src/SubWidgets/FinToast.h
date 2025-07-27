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
     * 부모 위젯이 있다면 부모 위젯 내부 상단에, 아니면 주 화면 우측 하단에 표시됩니다.
     * 토스트 메시지는 나온 곳으로 사라집니다.
     * 
     * @param inMessage 메시지 내용
     * @param inToastParent 메시지를 띄울 부모 위젯. 없다면 주 화면 우측 하단에 위치합니다.
     * @param inDuration 메시지 지속 시간. msec
     */
    static void showToast(const QString& inMessage, QWidget* inToastParent, const int inDuration = 1'500);

    /**
     * 토스트 메시지를 띄웁니다.
     * 대상 위젯 바깥쪽 상단 방향으로 띄우며, 상단 방향으로 사라집니다.
     * 
     * @param inMessage 메시지 내용
     * @param inTargetWidget 대상 위젯. 메시지는 대상 위젯 바깥 위쪽에 위치합니다.
     * @param inDuration 메시지 지속 시간. msec
     */
    static void popToastOnWidget(const QString& inMessage, QWidget* inTargetWidget, const int inDuration = 500);
};


#endif //FINTOAST_H
